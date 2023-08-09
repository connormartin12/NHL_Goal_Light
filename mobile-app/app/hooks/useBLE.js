import React, { useMemo, useState } from 'react';
import { BleError, BleManager, Characteristic, Device } from 'react-native-ble-plx';
import base64 from 'react-native-base64';

const ESP32_UUID = "FFFF";
const ESP32_SSID = "FF01";
const ESP32_PASSWORD = "FF02";
const ESP32_TEAM = "FF03";
const ESP32_DELAY = "FF04";

function useBLE() {
    const bleManager = useMemo(() => new BleManager(), []);

    const [allDevices, setAllDevices] = useState([]);
    const [connectedDevice, setConnectedDevice] = useState(null);
    
    const requestPermissions = async () => {
        // Add code here for android permissions

        return true;
    };

    const isDuplicateDevice = (devices, nextDevice) =>
        devices.findIndex((device) => nextDevice.id === device.id) > -1;

    const scanForPeripherals = () => {
        bleManager.startDeviceScan(null, null, (error, device) => {
            if (error) {
                console.log(error);
            }
            if(device && device.name?.includes("ESP32")) {
                setAllDevices((prevState) => {
                    if(!isDuplicateDevice(prevState, device)) {
                        return [...prevState, device]
                    }
                    return prevState;
                })
            }
        })
    };

    const connectToDevice = async (device) => {
        try {
            const deviceConnection = await bleManager.connectToDevice(device.id)
            setConnectedDevice(deviceConnection);
            await deviceConnection.discoverAllServicesAndCharacteristics();
            bleManager.stopDeviceScan();
            readData(deviceConnection);
        } catch (error) {
            console.log("Error Connecting", error);
        }
    };

    const readData = async (device) => {
        if (device) {
            device.readCharacteristicForService(ESP32_UUID, ESP32_DELAY)
                .then((characteristic) => {
                    const decodedValue = base64.decode(characteristic.value);
                    console.log(decodedValue);
                })
        } else {
            console.log("No device connected");
        }
    };

    const disconnectFromDevice = () => {
        if (connectedDevice) {
            bleManager.cancelDeviceConnection(connectedDevice.id);
            setConnectedDevice(null);
        }
    };

    return {
        allDevices,
        connectedDevice,
        connectToDevice,
        disconnectFromDevice,
        requestPermissions,
        scanForPeripherals,
    };
};

export default useBLE;