import React from 'react';
import { StyleSheet } from 'react-native';

import AppButton from '../components/AppButton';
import AppTextInput from '../components/AppTextInput';
import Screen from '../components/Screen';

function UserInputScreen() {
    const handleSubmit = () => console.log("Submitted");

    return (
        <Screen style={styles.container}>
            <AppTextInput placeholder="WiFi SSID" />
            <AppTextInput placeholder="WiFi Password" />
            <AppButton title="Submit" onPress={handleSubmit}/>
        </Screen>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        padding: 50,
    },
});

export default UserInputScreen;