import React, { useState } from 'react';
import { StyleSheet } from 'react-native';
import * as Yup from 'yup';

import { AppForm, AppFormField, AppFormPicker, AppFormSlider, SubmitButton } from '../components/forms';
import { colors, teams } from '../config';
import Screen from '../components/Screen';
import TeamPickerComponent from '../components/TeamPickerComponent';
import useBLE from '../hooks/useBLE';

import AppButton from '../components/AppButton';
import DeviceModal from '../components/DeviceConnectionModal';

let validationSchema = Yup.object({
    ssid: Yup.string().required().min(1).max(32).label("WiFi SSID"),
    password: Yup.string().required().min(1).max(63).label("WiFi Password"),
    team: Yup.object().required().label("Team"),
});

function UserInputScreen() {
    const {
        allDevices,
        connectedDevice,
        connectToDevice,
        disconnectFromDevice,
        requestPermissions,
        scanForPeripherals,
        writeData,
    } = useBLE();

    const [modalVisible, setModalVisible] = useState(false);

    const scanForDevices = async () => {
        const isPermissionsEnabled = await requestPermissions();
        if (isPermissionsEnabled) {
            scanForPeripherals();
        }
    }

    const hideModal = () => {
        setModalVisible(false);
    };

    const openModal = () => {
        scanForDevices();
        setModalVisible(true);
    };

    const handleSubmit = ( userData, { resetForm }) => {
        if (!connectedDevice) {
            alert("No bluetooth device connected");
            return;
        }

        writeData(ssid=userData.ssid, password=userData.password, teamSelection=userData.team.name, delay=userData.delay);
        console.log(userData);
        resetForm();
    };

    return (
        <Screen style={styles.container}>
            <AppButton title={connectedDevice? connectedDevice.name : "Select a Device"} onPress={connectedDevice ? disconnectFromDevice : openModal} />
            <DeviceModal
                closeModal={hideModal}
                connectToPeripheral={connectToDevice}
                devices={allDevices}
                visible={modalVisible}
            />

            <AppForm 
                initialValues={{
                    ssid: "",
                    password: "",
                    team: null,
                    delay: 30,
                }}
                onSubmit={handleSubmit}
                validationSchema={validationSchema}
            >
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    name="ssid"
                    placeholder="WiFi SSID" 
                    readOnly={connectedDevice? false : true}
                    width="100%"
                />
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    name="password"
                    placeholder="WiFi Password" 
                    readOnly={connectedDevice? false : true}
                    secureTextEntry={true} 
                    width="100%"
                />
                <AppFormPicker 
                    disabled={connectedDevice? false : true}
                    items={teams}
                    name="team"
                    numberOfColumns={1}
                    PickerItemComponent={TeamPickerComponent}
                    placeholder="Choose a Team"
                    width="100%"
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
                />
                <SubmitButton title="Submit" disabled={connectedDevice? false : true} />
            </AppForm>
        </Screen>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        padding: "10%",
    },
});

export default UserInputScreen;