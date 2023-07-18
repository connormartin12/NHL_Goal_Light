import React, { useState } from 'react';
import { StyleSheet, View } from 'react-native';

import AppButton from '../components/AppButton';
import AppTextInput from '../components/AppTextInput';
import Screen from '../components/Screen';

import TeamPicker from '../components/TeamPicker';
import DelaySlider from '../components/DelaySlider';

function UserInputScreen() {
    const handleSubmit = () => console.log("Submitted");

    return (
        <Screen style={styles.container}>

            <AppTextInput 
                autoCapitalize="none"
                autoCorrect={false}
                placeholder="WiFi SSID" 
            />
            <AppTextInput
                autoCapitalizee="none"
                autoCorrect={false}
                placeholder="WiFi Password" 
                secureTextEntry={true} 
            />
            <TeamPicker/>
            <DelaySlider/>
            <AppButton onPress={handleSubmit} title="Submit"/>
        </Screen>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        padding: '10%',
    },
});

export default UserInputScreen;