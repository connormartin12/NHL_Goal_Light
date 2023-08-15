import React, { useEffect, useState } from 'react';
import { StyleSheet } from 'react-native';
import * as Yup from 'yup';

import { AppForm, AppFormField, AppFormPicker, AppFormSlider, SubmitButton } from '../components/forms';
import { colors, teams } from '../config';
import Screen from '../components/Screen';
import TeamPickerComponent from '../components/TeamPickerComponent';
import useBLE from '../hooks/useBLE';

import AppButton from '../components/AppButton';
import AppText from '../components/AppText';
import cache from '../utilitiy/cache';
import DeviceModal from '../components/DeviceConnectionModal';

let validationSchema = Yup.object({
    ssid: Yup.string().required().min(1).max(32).label("WiFi SSID"),
    password: Yup.string().required().min(1).max(63).label("WiFi Password"),
    team: Yup.object().required().label("Team"),
});

function UserInputScreen( {navigation} ) {
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

    const [cachedData, setCachedData] = useState([]);

    const scanForDevices = async () => {
        const isPermissionsEnabled = await requestPermissions();
        if (isPermissionsEnabled) {
            scanForPeripherals();
        }
    };

    const hideModal = () => {
        setModalVisible(false);
    };

    const openModal = () => {
        scanForDevices();
        setModalVisible(true);
    };

    const retrieveData = async () => {
        try {
            const data = await cache.getData();
            setCachedData(data);
            return data;
        } catch (error) {
            console.log(error);
        }
    };

    useEffect(() => {
        retrieveData();
        openModal();
    }, []);

    const handleSubmit = ( userData, { resetForm }) => {
        if (!connectedDevice) {
            alert("No bluetooth device connected");
            return;
        }

        writeData(ssid=userData.ssid, password=userData.password, teamSelection=userData.team.name, delay=userData.delay);
        cache.storeData(userData);
        console.log(userData);
        resetForm();
    };

    return (
        <Screen style={styles.container}>
            <AppButton 
                onPress={connectedDevice ? disconnectFromDevice : openModal} 
                style={styles.connectionButton}
                title={connectedDevice? connectedDevice.name : "Select a Device"}
                />
            {!connectedDevice? <AppText style={styles.connectionWarning}>Connect to a Device</AppText> : null}
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
                    marginTop={20}
                    name="ssid"
                    placeholder="WiFi SSID" 
                    readOnly={connectedDevice? false : true}
                    width={300}
                />
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    name="password"
                    placeholder="WiFi Password" 
                    readOnly={connectedDevice? false : true}
                    secureTextEntry={true} 
                    width={300}
                />
                <AppFormPicker 
                    disabled={connectedDevice? false : true}
                    items={teams}
                    name="team"
                    numberOfColumns={1}
                    PickerItemComponent={TeamPickerComponent}
                    placeholder="Choose a Team"
                    width={300}
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
                    width={300}
                />
                <SubmitButton title="Submit" width={300} disabled={connectedDevice? false : true} />
            </AppForm>
        </Screen>
    );
};

const styles = StyleSheet.create({
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
        width: 300,
    },
    connectionWarning: {
        color: "red",
        marginVertical: 5,
    },
});

export default UserInputScreen;