/*
 * Prucho2 PIR ZG-204ZL - Z2M experimental external converter.
 * Uses standard Zigbee clusters from the Slacky-DIY firmware.
 * Not tested on physical hardware. Zigbee2MQTT 2.11+ requires:
 * advanced: {enable_external_js: true}.
 */
import {occupancy, battery} from 'zigbee-herdsman-converters/lib/modernExtend';
import {presets as e, access as ea} from 'zigbee-herdsman-converters/lib/exposes';

// v0.1.43+: firmware publishes illuminance manually on >=50 lx change.
// Do NOT use modernExtend illuminance() here: a stale/implicit configureReporting
// request for measuredValue fails with UNSUPPORTED_ATTRIBUTE and aborts configure.
const manualIlluminance = {
    cluster: 'msIlluminanceMeasurement',
    type: ['attributeReport', 'readResponse'],
    convert: (model, msg) => {
        const encoded = msg.data.measuredValue ?? msg.data[0] ?? msg.data['0'];
        if (typeof encoded !== 'number' || encoded === 0xFFFF) return {};
        const illuminance = encoded === 0 ? 0 : Math.round(Math.pow(10, (encoded - 1) / 10000));
        return {illuminance};
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
    convertGet: async (entity, key) => {
        // Ask for exactly one attribute per request. Reading all eleven at once
        // on a sleeping end-device previously caused 10-second ZCL timeout.
        const id = diagAttributes[key];
        if (id === undefined) throw new Error(`Unsupported PIR diagnostic key: ${key}`);
        await entity.read('msOccupancySensing', [id]);
    },
};


export default {
    zigbeeModel: ['Prucho2-PIR-z'],
    model: 'Prucho2-PIR-ZG204ZL',
    vendor: 'Prucho2 / Slacky-DIY',
    description: 'ZG-204ZL v0.1.43+ manual illuminance reporting (no configureReporting); TEST',
    extend: [
        occupancy({reporting: true, reportingConfig: {min: 'MIN', max: '1_HOUR', change: 0}}),
        // Manual illuminance reporting in device firmware; no ZCL configureReporting.
        battery({voltage: true}),
    ],
    fromZigbee: [manualIlluminance, pirDiagnostics],
    toZigbee: [occupancyTimeout, diagRead],
    exposes: [
        e.numeric('illuminance', ea.STATE).withUnit('lx')
            .withDescription('Illuminance, device-driven threshold reports; no configureReporting'),
        ...Object.keys(diagAttributes).map((name) => e.numeric(name, ea.STATE_GET).withDescription('Prucho2 PIR diagnostic ZCL counter, click refresh to read; v0.1.42')),
        e.numeric('occupancy_timeout', ea.ALL).withUnit('s')
            .withValueMin(1).withValueMax(600).withValueStep(1)
            .withDescription('Time from last PIR activity until unoccupied'),
    ],
};
