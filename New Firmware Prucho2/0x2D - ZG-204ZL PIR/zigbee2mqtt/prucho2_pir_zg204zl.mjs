/*
 * Prucho2 PIR ZG-204ZL - Z2M experimental external converter.
 * Uses standard Zigbee clusters from the Slacky-DIY firmware.
 * Not tested on physical hardware. Zigbee2MQTT 2.11+ requires:
 * advanced: {enable_external_js: true}.
 */
import {occupancy, illuminance, battery} from 'zigbee-herdsman-converters/lib/modernExtend';
import {presets as e, access as ea} from 'zigbee-herdsman-converters/lib/exposes';

// v0.1.42: explicit numeric reads from Zigbee attributes absent from the Z2M
// Dev console attribute-name dropdown. Not manufacturer-specific ZCL frames.
const diagAttributes = {
    pir_gpio: 0xF100,
    pir_rising_edges: 0xF101,
    pir_falling_edges: 0xF102,
    pir_reports_requested: 0xF103,
    pir_reports_enqueued: 0xF104,
    pir_reports_retries: 0xF105,
    pir_timer_checks: 0xF106,
    pir_high_samples: 0xF107,
    pir_low_samples: 0xF108,
    pir_sleep_returns: 0xF109,
    pir_poll_samples: 0xF10A,
    pir_pd_in: 0xF10B,
    pir_pd_ie: 0xF10C,
    pir_pd_oen: 0xF10D,
    pir_pd_gpio: 0xF10E,
    pir_pd_out: 0xF10F,
    pir_pd_irq_en: 0xF110,
};
const luxDiagAttributes = {
    lux_timer_ticks: 0xF200,
    lux_samples: 0xF201,
    lux_adc_vbat: 0xF202,
    lux_adc_pc4: 0xF203,
    lux_zcl_value: 0xF204,
    lux_report_attempts: 0xF205,
    lux_reports_enqueued: 0xF206,
    lux_reports_failed: 0xF207,
    lux_diag_snapshots: 0xF208,
    lux_status: 0xF209,
};
const lightDiagnostics = {
    cluster: 'msIlluminanceMeasurement',
    type: ['readResponse', 'attributeReport'],
    convert: (model, msg) => {
        const result = {};
        for (const [name, id] of Object.entries(luxDiagAttributes)) {
            const value = msg.data[id] ?? msg.data[String(id)] ?? msg.data['0x' + id.toString(16)];
            if (typeof value === 'number') result[name] = value;
        }
        return result;
    },
};
const luxDiagRead = {
    key: Object.keys(luxDiagAttributes),
    convertGet: async (entity, key) => {
        await entity.read('msIlluminanceMeasurement', [luxDiagAttributes[key]]);
    },
};
const pirDiagnostics = {
    cluster: 'msOccupancySensing',
    type: ['readResponse', 'attributeReport'],
    convert: (model, msg) => {
        const result = {};
        for (const [name, id] of Object.entries(diagAttributes)) {
            const value = msg.data[id] ?? msg.data[String(id)] ?? msg.data['0x' + id.toString(16)];
            if (typeof value === 'number') result[name] = value;
        }
        return result;
    },
};
const diagRead = {
    key: Object.keys(diagAttributes),
    convertGet: async (entity, key) => {
        // Ask for exactly one attribute per request. Reading all eleven at once
        // on a sleeping end-device previously caused 10-second ZCL timeout.
        const id = diagAttributes[key];
        if (id === undefined) throw new Error(`Unsupported PIR diagnostic key: ${key}`);
        await entity.read('msOccupancySensing', [id]);
    },
};



const occupancyTimeout = {
    key: ['occupancy_timeout'],
    convertSet: async (entity, key, value) => {
        const seconds = Number(value);
        if (!Number.isInteger(seconds) || seconds < 1 || seconds > 600) {
            throw new Error('occupancy_timeout must be between 1 and 600 seconds');
        }
        await entity.write('msOccupancySensing', {pirOToUDelay: seconds});
        return {state: {occupancy_timeout: seconds}};
    },
    convertGet: async (entity) => {
        await entity.read('msOccupancySensing', ['pirOToUDelay']);
    },
};

export default {
    zigbeeModel: ['Prucho2-PIR-z'],
    model: 'Prucho2-PIR-ZG204ZL',
    vendor: 'Prucho2 / Slacky-DIY',
    description: 'ZG-204ZL Telink PIR with optional populated lux components; one identical firmware for all boards',
    extend: [
        occupancy({reporting: true, reportingConfig: {min: 'MIN', max: '1_HOUR', change: 0}}),
        illuminance({reporting: {min: '10_SECONDS', max: '1_HOUR', change: 10}}),
        battery({voltage: true}),
    ],
    fromZigbee: [pirDiagnostics, lightDiagnostics],
    toZigbee: [occupancyTimeout, diagRead, luxDiagRead],
    exposes: [
        ...Object.keys(luxDiagAttributes).map((name) => e.numeric(name, ea.STATE_GET)
            .withDescription('Light diagnostic attribute; snapshot every ~15s')),
        ...Object.keys(diagAttributes).map((name) => e.numeric(name, ea.STATE_GET)
            .withDescription('PIR diagnostic and raw GPIO register; snapshot every ~10s or on-demand get')),
        e.numeric('occupancy_timeout', ea.ALL).withUnit('s')
            .withValueMin(1).withValueMax(600).withValueStep(1)
            .withDescription('Time from last PIR activity until unoccupied'),
    ],
};
