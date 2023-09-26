import React, { useMemo, useState } from 'react';
import { BleError, BleManager, Characteristic, Device } from 'react-native-ble-plx';
import base64 from 'react-native-base64';

import teams from '../config/teams';

const ESP32_UUID = "FFFF";
const ESP32_SSID = "FF01";
const ESP32_PASSWORD = "FF02";
const ESP32_TEAM = "FF03";
const ESP32_DELAY = "FF04";

/* The characteristic values read from the esp32 are strings in a null terminated
   format. This function determines where the null termination is so the string value
   can be displayed properly as the initial form values. */
function stringVal(string) {
    for (let i = 0; i<string.length; i++) {
        if (string.charCodeAt(i) === 0) {
            return(string.slice(0, i));
        }
    }
};

function useBLE() {
    const bleManager = useMemo(() => new BleManager(), []);

    const [allDevices, setAllDevices] = useState([]);
    const [connectedDevice, setConnectedDevice] = useState(null);

    const [espSSID, setEspSSID] = useState("");
    const [espPassword, setEspPassword] = useState("");
    const [espTeam, setEspTeam] = useState(null);
    const [espDelay, setEspDelay] = useState(30);
    
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
            if(device && device.name?.includes("Goal Light")) {
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
            device.readCharacteristicForService(ESP32_UUID, ESP32_SSID)
                .then((characteristic) => {
                    const decodedValue = base64.decode(characteristic.value);
                    setEspSSID(stringVal(decodedValue));
                })
            device.readCharacteristicForService(ESP32_UUID, ESP32_PASSWORD)
                .then((characteristic) => {
                    const decodedValue = base64.decode(characteristic.value);
                    setEspPassword(stringVal(decodedValue));
                })
            device.readCharacteristicForService(ESP32_UUID, ESP32_TEAM)
                .then((characteristic) => {
                    const decodedValue = base64.decode(characteristic.value);
                    const idIndex = decodedValue.indexOf(`"id":`);
                    let teamId = decodedValue.slice(idIndex+5, idIndex+7);
                    if (isNaN(teamId[1]) == true)
                        teamId = teamId.slice(0, 1);
                    teams.forEach(team => {
                        if (team.id == teamId)
                            setEspTeam(team);
                    });
                })
            device.readCharacteristicForService(ESP32_UUID, ESP32_DELAY)
                .then((characteristic) => {
                    const decodedValue = base64.decode(characteristic.value);
                    setEspDelay(parseInt(decodedValue));
                })
        } else {
            console.log("No device connected");
        }
    };

    const writeData = async ( ssid, password, teamSelection, delay ) => {
        const SSID = base64.encode(ssid);
        const Password = base64.encode(password);
        const Team = base64.encode(JSON.stringify(teamSelection));
        const Delay = base64.encode(delay.toString());

        if (connectedDevice) {
            connectedDevice.writeCharacteristicWithResponseForService(ESP32_UUID, ESP32_SSID, SSID)
                .then(() => {
                    console.log("SSID Written")
                })
            connectedDevice.writeCharacteristicWithResponseForService(ESP32_UUID, ESP32_PASSWORD, Password)
                .then(() => {
                    console.log("Password Written")
                })
            connectedDevice.writeCharacteristicWithResponseForService(ESP32_UUID, ESP32_TEAM, Team)
                .then(() => {
                    console.log("Team Written")
                })
            connectedDevice.writeCharacteristicWithResponseForService(ESP32_UUID, ESP32_DELAY, Delay)
                .then(() => {
                    console.log("Delay Written")
                })
        } else {
            console.log("No device connected");
        }
    }

    const disconnectFromDevice = () => {
        if (connectedDevice) {
            if (connectedDevice == null) {
                bleManager.cancelDeviceConnection(connectedDevice.id);
            }
            setConnectedDevice(null);
            setEspSSID("");
            setEspPassword("");
            setEspTeam(null);
            setEspDelay(30);
        }
    };

    return {
        allDevices,
        connectedDevice,
        connectToDevice,
        disconnectFromDevice,
        espDelay,
        espPassword,
        espSSID,
        espTeam,
        requestPermissions,
        scanForPeripherals,
        writeData,
    };
};

export default useBLE;