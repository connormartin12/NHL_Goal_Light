import React, { useEffect, useState } from 'react';
import { StyleSheet } from 'react-native';
import * as Yup from 'yup';

// Remove with better styling
import { Modal, Pressable, Text } from 'react-native';

import AdditionalSettingsModal, { additionalSettingsModal } from '../components/AdditionalSettingsModal';
import { AppForm, AppFormField, AppFormPicker, AppFormSlider, SubmitButton } from '../components/forms';
import { colors, teams } from '../config';
import Screen from '../components/Screen';
import TeamPickerComponent from '../components/TeamPickerComponent';
import useBLE from '../hooks/useBLE';

import AppButton from '../components/AppButton';
import AppText from '../components/AppText';
import DeviceModal from '../components/DeviceConnectionModal';

let validationSchema = Yup.object({
    ssid: Yup.string().required().min(1).max(32).label("WiFi SSID"),
    password: Yup.string().required().min(1).max(63).label("WiFi Password"),
    team: Yup.object().required().label("Team"),
});

const inputFieldWidth = 300;

function UserInputScreen( {navigation} ) {
    const {
        allDevices,
        connectedDevice,
        connectToDevice,
        disconnectFromDevice,
        espDelay,
        espPassword,
        espSSID,
        espTeam,
        requestPermissions,
        resetDevice,
        scanForPeripherals,
        sendVolumeSetting,
        writeData,
    } = useBLE();

    const [deviceModalVisible, setDeviceModalVisible] = useState(false);
    const [settingsModalVisible, setSettingsModalVisible] = useState(false);
    const [passwordVisible, setPasswordVisible] = useState(true);

    const scanForDevices = async () => {
        const isPermissionsEnabled = await requestPermissions();
        if (isPermissionsEnabled) {
            scanForPeripherals();
        }
    };

    const hideModal = () => {
        setDeviceModalVisible(false);
        setSettingsModalVisible(false);
    };

    const openModal = () => {
        scanForDevices();
        setDeviceModalVisible(true);
    };

    useEffect(() => {
        openModal();
    }, []);

    const additionalSettingsModal = () => {
        setSettingsModalVisible(true);
    };

    const handleSubmit = ( userData, { resetForm }) => {
        if (!connectedDevice) {
            alert("No bluetooth device connected");
            return;
        }

        writeData(ssid=userData.ssid, password=userData.password, 
                  teamSelection=userData.team, delay=userData.delay);
        disconnectFromDevice();
        resetForm();
    };

    return (
        <Screen style={styles.container}>
            <AppButton 
                onPress={connectedDevice ? disconnectFromDevice : openModal} 
                style={styles.connectionButton}
                title={connectedDevice? connectedDevice.name : "Select a Device"}
                />
            {!connectedDevice? (<AppText style={styles.connectionWarning}>Connect to a Device</AppText>) : null}
            <DeviceModal
                closeModal={hideModal}
                connectToPeripheral={connectToDevice}
                devices={allDevices}
                visible={deviceModalVisible}
            />

            <AppForm 
                enableReinitialize={true}
                initialValues={{
                    ssid: espSSID,
                    password: espPassword,
                    team: espTeam,
                    delay: espDelay,
                }}
                onSubmit={handleSubmit}
                validationSchema={validationSchema}
            >
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    marginTop={20}
                    name="ssid"
                    placeholder="WiFi SSID" 
                    readOnly={connectedDevice? false : true}
                    width={inputFieldWidth}
                />
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    name="password"
                    onPress={() => setPasswordVisible(!passwordVisible)}
                    passwordField={true}
                    placeholder="WiFi Password" 
                    readOnly={connectedDevice? false : true}
                    secureTextEntry={passwordVisible} 
                    width={inputFieldWidth}
                />
                <AppFormPicker 
                    disabled={connectedDevice? false : true}
                    items={teams}
                    name="team"
                    numberOfColumns={1}
                    PickerItemComponent={TeamPickerComponent}
                    placeholder="Choose a Team"
                    width={inputFieldWidth}
                />
                <AppFormSlider
                    disabled={connectedDevice? false : true}
                    minimumTrackTintColor={colors.primary}
                    minimumValue={0}
                    maximumValue={60}
                    name="delay"
                    step={1}
                    valuePrefix="Goal Light Delay:"
                    valueSuffix="Seconds"
                    width={inputFieldWidth}
                />
                <Pressable 
                    disabled={connectedDevice? false : true} 
                    onPress={additionalSettingsModal}
                    style={styles.additionalSettings}
                >
                    <Text style={styles.additionalSettingsText}>Additional Settings</Text>
                </Pressable>
                <AdditionalSettingsModal 
                    closeModal={hideModal}
                    disconnectFromPeripheral={disconnectFromDevice}
                    resetPeripheral={resetDevice}
                    sendVolumeSetting={sendVolumeSetting}
                    visible={settingsModalVisible}
                />
                <SubmitButton 
                    title="Save Settings" 
                    width={inputFieldWidth} 
                    disabled={connectedDevice? false : true}
                />
            </AppForm>
        </Screen>
    );
};

const styles = StyleSheet.create({
    additionalSettings: {
        marginTop: 24,
    },
    additionalSettingsText: {
        fontSize: 20,
        textDecorationLine: 'underline',
    },
    container: {
        flex: 1,
        justifyContent: 'center',
        padding: "10%",
        alignItems: 'center'
    },
    connectionButton: {
        alignItems: 'center',
        backgroundColor: colors.primary,
        borderRadius: 25,
        justifyContent: 'center',
        marginVertical: 0,
        padding: 15,
        width: inputFieldWidth,
    },
    connectionWarning: {
        color: "red",
        marginVertical: 5,
    },
});

export default UserInputScreen;