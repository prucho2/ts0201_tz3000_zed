/*
 * Prucho2 PIR ZG-204ZL - Z2M experimental external converter.
 * Uses standard Zigbee clusters from the Slacky-DIY firmware.
 * Not tested on physical hardware. Zigbee2MQTT 2.11+ requires:
 * advanced: {enable_external_js: true}.
 */
import {occupancy, illuminance, battery} from 'zigbee-herdsman-converters/lib/modernExtend';
import {presets as e, access as ea} from 'zigbee-herdsman-converters/lib/exposes';

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
    convertGet: async (entity) => {
        // Read all counters in one request; Zigbee return publishes a readResponse.
        await entity.read('msOccupancySensing', Object.values(diagAttributes));
    },
};


// v0.1.42 TEST-only diagnostic read trigger.
// Sending {"pir_diagnostics":1} to pir-prucho/set invokes ZCL READ only,
// never writes any Zigbee attribute or changes occupancy.
// Values are included in the returned Zigbee2MQTT state.
const pirDiagnosticNames = {
    0xF100: 'pir_gpio_level',
    0xF101: 'pir_rising_edges',
    0xF102: 'pir_falling_edges',
    0xF103: 'pir_reports_requested',
    0xF104: 'pir_reports_enqueued',
    0xF105: 'pir_report_retries',
    0xF106: 'pir_timer_checks',
    0xF107: 'pir_high_samples',
    0xF108: 'pir_low_samples',
    0xF109: 'pir_sleep_returns',
    0xF10A: 'pir_poll_samples',
};
const pirDiagnostics = {
    key: ['pir_diagnostics'],
    convertSet: async (entity, key, value) => {
        if (Number(value) !== 1) throw new Error('pir_diagnostics requires value 1');
        const ids = Object.keys(pirDiagnosticNames).map(Number);
        const response = await entity.read('msOccupancySensing', ids);
        const state = {};
        for (const id of ids) {
            const k = pirDiagnosticNames[id];
            const value = response[id] ?? response[String(id)] ?? response[`0x${id.toString(16)}`];
            if (value !== undefined) state[k] = value;
        }
        state.pir_diagnostics = 0;
        return {state};
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
    fromZigbee: [pirDiagnostics],
    toZigbee: [occupancyTimeout, diagRead],
    exposes: [
        ...Object.keys(diagAttributes).map((name) => e.numeric(name, ea.STATE_GET).withDescription('Prucho2 PIR diagnostic ZCL counter, click refresh to read; v0.1.42')),
        e.numeric('occupancy_timeout', ea.ALL).withUnit('s')
            .withValueMin(1).withValueMax(600).withValueStep(1)
            .withDescription('Time from last PIR activity until unoccupied'),
    ],
};
