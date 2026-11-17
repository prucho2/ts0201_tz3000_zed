/*
 * Prucho2 PIR ZG-204ZL - Slacky-DIY-based Zigbee2MQTT external converter.
 * Stable occupancy, illuminance and battery only; diagnostics removed.
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

export default {
    zigbeeModel: ['Prucho2-PIR-z'],
    model: 'Prucho2-PIR-ZG204ZL',
    vendor: 'Prucho2 / Slacky-DIY',
    description: 'ZG-204ZL PIR with optional lux sensor; 20 s lux sampling, 20 lx change, 300 s sync',
    extend: [
        occupancy({reporting: true, reportingConfig: {min: 'MIN', max: '1_HOUR', change: 0}}),
        illuminance({reporting: {min: '10_SECONDS', max: '1_HOUR', change: 10}}),
        battery({voltage: true}),
    ],
    toZigbee: [occupancyTimeout],
    exposes: [
        e.numeric('occupancy_timeout', ea.ALL).withUnit('s')
            .withValueMin(1).withValueMax(600).withValueStep(1)
            .withDescription('Hold after PIR activity; firmware default 3 s'),
    ],
};
