/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2013 Semtech-Cycleo

Description:
    LoRa concentrator : Just In Time TX scheduling queue

License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Michael Coracin
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdio.h>      /* printf, fprintf, snprintf, fopen, fputs */
#include <string.h>     /* memset, memcpy */
#include <pthread.h>
#include <assert.h>
#include <math.h>

#include "trace.h"
#include "jitqueue.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS & TYPES -------------------------------------------- */
#define TX_START_DELAY          1500    /* microseconds */
                                        /* TODO: get this value from HAL? */
#define TX_MARGIN_DELAY         1000    /* Packet overlap margin in microseconds */
                                        /* TODO: How much margin should we take? */
#define TX_JIT_DELAY            30000   /* Pre-delay to program packet for TX in microseconds */
#define TX_MAX_ADVANCE_DELAY    ((JIT_NUM_BEACON_IN_QUEUE + 1) * 128 * 1E6) /* Maximum advance delay accepted for a TX packet, compared to current time */

#define BEACON_GUARD            3000000 /* Interval where no ping slot can be placed,
                                            to ensure beacon can be sent */
#define BEACON_RESERVED         2120000 /* Time on air of the beacon, with some margin */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES (GLOBAL) ------------------------------------------- */
static pthread_mutex_t mx_jit_queue = PTHREAD_MUTEX_INITIALIZER; /* control access to JIT queue */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */
static uint32_t time_on_air(struct lgw_pkt_tx_s *packet, bool isBeacon) {
    uint8_t SF, H, DE;
    uint16_t BW;
    uint32_t payloadSymbNb, Tpacket;
    double Tsym, Tpreamble, Tpayload;

    switch (packet->bandwidth) {
        case BW_125KHZ:
            BW = 125;
            break;
        case BW_250KHZ:
            BW = 250;
            break;
        case BW_500KHZ:
            BW = 500;
            break;
        default:
            MSG("ERROR: Cannot compute time on air for this packet, unsupported bandwidth (%u)\n", packet->bandwidth);
            return 0;
    }

    switch (packet->datarate) {
        case DR_LORA_SF7:
            SF = 7;
            break;
        case DR_LORA_SF8:
            SF = 8;
            break;
        case DR_LORA_SF9:
            SF = 9;
            break;
        case DR_LORA_SF10:
            SF = 10;
            break;
        case DR_LORA_SF11:
            SF = 11;
            break;
        case DR_LORA_SF12:
            SF = 12;
            break;
        default:
            MSG("ERROR: Cannot compute time on air for this packet, unsupported datarate (%u)\n", packet->datarate);
            return 0;
    }

    /* Duration of 1 symbol */
    Tsym = pow(2, SF) / BW;

    /* Duration of preamble */
    Tpreamble = (8 + 4.25) * Tsym; /* 8 programmed symbols in preamble */

    /* Duration of payload */
    H = (isBeacon==false)?0:1; /* header is always enabled, except for beacons */
    DE = (SF >= 11)?1:0; /* Low datarate optimization enabled for SF11 and SF12 */

    payloadSymbNb = 8 + (ceil((double)(8*packet->size - 4*SF + 28 + 16 - 20*H) / (double)(4*(SF - 2*DE))) * (packet->coderate + 4)); /* Explicitely cast to double to keep precision of the division */

    Tpayload = payloadSymbNb * Tsym;

    Tpacket = Tpreamble + Tpayload;

    return Tpacket;
}

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ----------------------------------------- */

bool jit_queue_is_full(struct jit_queue_s *queue) {
    bool result;

    pthread_mutex_lock(&mx_jit_queue);

    result = (queue->num_pkt == JIT_QUEUE_MAX)?true:false;

    pthread_mutex_unlock(&mx_jit_queue);

    return result;
}

bool jit_queue_is_empty(struct jit_queue_s *queue) {
    bool result;

    pthread_mutex_lock(&mx_jit_queue);

    result = (queue->num_pkt == 0)?true:false;

    pthread_mutex_unlock(&mx_jit_queue);

    return result;
}

void jit_queue_init(struct jit_queue_s *queue) {
    int i;

    pthread_mutex_lock(&mx_jit_queue);

    memset(queue, 0, sizeof(*queue));
    for (i=0; i<JIT_QUEUE_MAX; i++) {
        queue->nodes[i].is_sent = true; /* indicates that slot is available */
        queue->nodes[i].pre_delay = 0;
        queue->nodes[i].post_delay = 0;
    }

    pthread_mutex_unlock(&mx_jit_queue);
}

enum jit_error_e jit_enqueue(struct jit_queue_s *queue, struct timeval *time, struct lgw_pkt_tx_s *packet, enum jit_pkt_type_e pkt_type) {
    int i = 0;
    uint32_t time_us = time->tv_sec * 1000000UL + time->tv_usec; /* convert time in µs */
    uint32_t packet_post_delay = 0;
    uint32_t packet_pre_delay = 0;
    uint32_t target_pre_delay = 0;
    enum jit_error_e err_collision;

    MSG_DEBUG(DEBUG_JIT, "time=%u µs\n", time_us);

    if (packet == NULL) {
        MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: invalid parameter\n");
        return JIT_ERROR_INVALID;
    }

    if (jit_queue_is_full(queue)) {
        MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: cannot enqueue packet, JIT queue is full\n");
        return JIT_ERROR_FULL;
    }

    /* Compute packet pre/post delays depending on packet's type */
    switch (pkt_type) {
        case JIT_PKT_TYPE_DOWNLINK_CLASS_A:
        case JIT_PKT_TYPE_DOWNLINK_CLASS_B:
            packet_pre_delay = TX_START_DELAY;
            packet_post_delay = time_on_air(packet, false) * 1000UL; /* in us */
            break;
        case JIT_PKT_TYPE_BEACON:
            /* As defined in LoRaWAN spec */
            packet_pre_delay = TX_START_DELAY + BEACON_GUARD;
            packet_post_delay = BEACON_RESERVED;
            break;
        default:
            break;
    }

    /* Check criteria_1: is it already too late to send this packet ?
     *  The packet should arrive at least at (tmst - TX_START_DELAY) to be programmed into concentrator
     *  Note: - Also add some margin, to be checked how much is needed, if needed
     *        - Valid for both Downlinks and Beacon packets
     *
     *  Warning: unsigned arithmetic (handle roll-over)
     *      t_packet < t_current + TX_START_DELAY + MARGIN
     */
    if ((packet->count_us - time_us) <= (TX_START_DELAY + TX_MARGIN_DELAY)) {
        MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: Packet REJECTED, already too late to send it (current=%u, packet=%u, type=%d)\n", time_us, packet->count_us, pkt_type);
        return JIT_ERROR_TOO_LATE;
    }

    /* Check criteria_2: Does packet timestamp seem plausible compared to current time
     *  We do not expect the server to program a downlink too early compared to current time
     *  Class A: downlink has to be sent in a 1s or 2s time window after RX
     *  Class B: downlink has to occur in a 128s time window
     *  So let's define a safe delay above which we can say that the packet is out of bound: TX_MAX_ADVANCE_DELAY
     *  Note: - Valid for Downlinks only, not for Beacon packets
     *
     *  Warning: unsigned arithmetic (handle roll-over)
                t_packet > t_current + TX_MAX_ADVANCE_DELAY
     */
    if ((pkt_type == JIT_PKT_TYPE_DOWNLINK_CLASS_A) || (pkt_type == JIT_PKT_TYPE_DOWNLINK_CLASS_B)) {
        if ((packet->count_us - time_us) > TX_MAX_ADVANCE_DELAY) {
            MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: Packet REJECTED, timestamp seems wrong, too much in advance (current=%u, packet=%u, type=%d)\n", time_us, packet->count_us, pkt_type);
            return JIT_ERROR_TOO_EARLY;
        }
    }

    pthread_mutex_lock(&mx_jit_queue);

    /* Check criteria_3: does this new packet overlap with a packet already enqueued ?
     *  Note: - need to take into account packet's pre_delay and post_delay of each packet
     *        - Valid for both Downlinks and beacon packets
     *        - Beacon guard can be ignored if we try to queue a Class A downlink
     */
    for (i=0; i<JIT_QUEUE_MAX; i++) {
        if (!queue->nodes[i].is_sent) {
            /* We ignore Beacon Guard for Class A downlinks */
            if ((pkt_type == JIT_PKT_TYPE_DOWNLINK_CLASS_A) &&
                (queue->nodes[i].pkt_type == JIT_PKT_TYPE_BEACON)) {
                target_pre_delay = TX_START_DELAY;
            } else {
                target_pre_delay = queue->nodes[i].pre_delay;
            }

            /* Check if there is a collision
             *  Warning: unsigned arithmetic (handle roll-over)
             *      t_packet_new - pre_delay_packet_new < t_packet_prev + post_delay_packet_prev (OVERLAP on post delay)
             *      t_packet_new + post_delay_packet_new > t_packet_prev - pre_delay_packet_prev (OVERLAP on pre delay)
             */
            if (((packet->count_us - queue->nodes[i].pkt.count_us) <= (packet_pre_delay + queue->nodes[i].post_delay + TX_MARGIN_DELAY)) || ((queue->nodes[i].pkt.count_us - packet->count_us) <= (target_pre_delay + packet_post_delay + TX_MARGIN_DELAY))) {
                switch (queue->nodes[i].pkt_type) {
                    case JIT_PKT_TYPE_DOWNLINK_CLASS_A:
                    case JIT_PKT_TYPE_DOWNLINK_CLASS_B:
                        MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: Packet (type=%d) REJECTED, collision with packet already programmed at %u (%u)\n", pkt_type, queue->nodes[i].pkt.count_us, packet->count_us);
                        err_collision = JIT_ERROR_COLLISION_PACKET;
                        break;
                    case JIT_PKT_TYPE_BEACON:
                        if (pkt_type != JIT_PKT_TYPE_BEACON) {
                            /* do not overload logs for beacon/beacon collision, as it is expected to happen with beacon pre-scheduling algorith used */
                            MSG_DEBUG(DEBUG_JIT_ERROR, "ERROR: Packet (type=%d) REJECTED, collision with beacon already programmed at %u (%u)\n", pkt_type, queue->nodes[i].pkt.count_us, packet->count_us);
                        }
                        err_collision = JIT_ERROR_COLLISION_BEACON;
                        break;
                    default:
                        MSG("ERROR: Unknown packet type, should not occur, BUG!\n");
                        assert(0);
                        break;
                }
                pthread_mutex_unlock(&mx_jit_queue);
                return err_collision;
            }
        }
    }

    /* Finally enqueue it */
    for (i=0; i<JIT_QUEUE_MAX; i++) {
        if (queue->nodes[i].is_sent) { /* slot is available */
            memcpy(&(queue->nodes[i].pkt), packet, sizeof(struct lgw_pkt_tx_s));
            queue->nodes[i].is_sent = false;
            queue->nodes[i].pre_delay = packet_pre_delay;
            queue->nodes[i].post_delay = packet_post_delay;
            queue->nodes[i].pkt_type = pkt_type;
            if (pkt_type == JIT_PKT_TYPE_BEACON) {
                queue->num_beacon++;
            }
            queue->num_pkt++;
            break;
        }
    }

    pthread_mutex_unlock(&mx_jit_queue);

    jit_print_queue(queue, false, DEBUG_JIT);

    MSG_DEBUG(DEBUG_JIT, "enqueued packet with count_us=%u at index %d (size = %u bytes, toa = %u us)\n", packet->count_us, i, packet->size, packet_post_delay); 

    return JIT_ERROR_OK;
}

enum jit_error_e jit_dequeue(struct jit_queue_s *queue, int index, struct lgw_pkt_tx_s *packet, enum jit_pkt_type_e *pkt_type) {
    if (packet == NULL) {
        MSG("ERROR: invalid parameter\n");
        return JIT_ERROR_INVALID;
    }

    if ((index < 0) || (index >= JIT_QUEUE_MAX)) {
        MSG("ERROR: invalid parameter\n");
        return JIT_ERROR_INVALID;
    }

    if (jit_queue_is_empty(queue)) {
        MSG("ERROR: cannot dequeue packet, JIT queue is empty\n");
        return JIT_ERROR_EMPTY;
    }

    pthread_mutex_lock(&mx_jit_queue);

    /* Dequeue requested packet */
    memcpy(packet, &(queue->nodes[index].pkt), sizeof(struct lgw_pkt_tx_s));
    queue->nodes[index].is_sent = true;
    queue->num_pkt--;
    *pkt_type = queue->nodes[index].pkt_type;
    if (*pkt_type == JIT_PKT_TYPE_BEACON) {
        queue->num_beacon--;
        MSG_DEBUG(DEBUG_BEACON, "--- Beacon dequeued ---\n");
    }

    pthread_mutex_unlock(&mx_jit_queue);

    jit_print_queue(queue, false, DEBUG_JIT);

    MSG_DEBUG(DEBUG_JIT, "dequeued packet with count_us=%u from index %d\n", packet->count_us, index);

    return JIT_ERROR_OK;
}

enum jit_error_e jit_peek(struct jit_queue_s *queue, struct timeval *time, int *pkt_idx) {
    /* Return index of node containing a packet inline with given time */
    int i = 0;
    int idx_highest_priority = -1;
    uint32_t time_us;

    if ((time == NULL) || (pkt_idx == NULL)) {
        MSG("ERROR: invalid parameter\n");
        return JIT_ERROR_INVALID;
    }

    if (jit_queue_is_empty(queue)) {
        return JIT_ERROR_EMPTY;
    }

    time_us = time->tv_sec * 1000000UL + time->tv_usec;

    pthread_mutex_lock(&mx_jit_queue);

    /* Search for highest priority packet to be sent */
    for (i=0; i<JIT_QUEUE_MAX; i++) {
        if (!queue->nodes[i].is_sent) { /* there is a packet to be sent */
            /* First check if that packet is outdated:
             *  If a packet seems too much in advance, and was not rejected at enqueue time,
             *  it means that we missed it for peeking, we need to drop it
             *
             *  Warning: unsigned arithmetic
             *      t_packet > t_current + TX_MAX_ADVANCE_DELAY
             */
            if ((queue->nodes[i].pkt.count_us - time_us) >= TX_MAX_ADVANCE_DELAY) {
                /* We drop the packet to avoid lock-up */
                queue->nodes[i].is_sent = true;
                queue->num_pkt--;
                if (queue->nodes[i].pkt_type == JIT_PKT_TYPE_BEACON) {
                    queue->num_beacon--;
                    MSG("WARNING: --- Beacon dropped (current_time=%u, packet_time=%u) ---\n", time_us, queue->nodes[i].pkt.count_us);
                } else {
                    MSG("WARNING: --- Packet dropped (current_time=%u, packet_time=%u) ---\n", time_us, queue->nodes[i].pkt.count_us);
                }
            }

            /* Then look for highest priority packet to be sent:
             *  Warning: unsigned arithmetic (handle roll-over)
             *      t_packet < t_highest
             */
            if ((idx_highest_priority == -1) || ((queue->nodes[i].pkt.count_us - time_us) < (queue->nodes[idx_highest_priority].pkt.count_us - time_us))) {
                idx_highest_priority = i;
            }
        }
    }

    /* Peek criteria 1: look for a packet to be sent in next TX_JIT_DELAY ms timeframe
     *  Warning: unsigned arithmetic (handle roll-over)
     *      t_packet < t_current + TX_JIT_DELAY
     */
    if ((queue->nodes[idx_highest_priority].pkt.count_us - time_us) < TX_JIT_DELAY) {
        *pkt_idx = idx_highest_priority;
        MSG_DEBUG(DEBUG_JIT, "peek packet with count_us=%u at index %d\n",
            queue->nodes[idx_highest_priority].pkt.count_us, idx_highest_priority);
    } else {
        *pkt_idx = -1;
    }

    pthread_mutex_unlock(&mx_jit_queue);

    return JIT_ERROR_OK;
}

void jit_print_queue(struct jit_queue_s *queue, bool show_sent, int debug_level) {
    int i = 0;

    if (jit_queue_is_empty(queue)) {
        MSG_DEBUG(debug_level, "INFO: [jit] queue is empty\n");
    } else {
        pthread_mutex_lock(&mx_jit_queue);

        MSG_DEBUG(debug_level, "INFO: [jit] queue contains %d packets:\n", queue->num_pkt);
        MSG_DEBUG(debug_level, "INFO: [jit] queue contains %d beacons:\n", queue->num_beacon);
        for (i=0; i<JIT_QUEUE_MAX; i++) {
            if ((queue->nodes[i].is_sent == false) || show_sent) {
                MSG_DEBUG(debug_level, " - node[%d]: is_sent=%d - count_us=%u - type=%d\n",
                        i, queue->nodes[i].is_sent,
                        queue->nodes[i].pkt.count_us,
                        queue->nodes[i].pkt_type);
                }
        }

        pthread_mutex_unlock(&mx_jit_queue);
    }
}

