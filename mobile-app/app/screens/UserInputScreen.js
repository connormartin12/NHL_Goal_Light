import React from 'react';
import { StyleSheet } from 'react-native';
import * as Yup from 'yup';

import { AppForm, AppFormField, AppFormPicker, AppFormSlider, SubmitButton } from '../components/forms';
import { colors, teams } from '../config';
import Screen from '../components/Screen';
import TeamPickerComponent from '../components/TeamPickerComponent';
import useBLE from '../hooks/useBLE';

let validationSchema = Yup.object({
    ssid: Yup.string().required().min(1).max(32).label("WiFi SSID"),
    password: Yup.string().required().min(1).max(63).label("WiFi Password"),
    team: Yup.object().required().label("Team"),
});

function UserInputScreen() {
    const handleSubmit = ( userData, { resetForm }) => {
        console.log(userData);
        resetForm();
    };

    return (
        <Screen style={styles.container}>
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
                    width="100%"
                />
                <AppFormField
                    autoCapitalize="none"
                    autoCorrect={false}
                    name="password"
                    placeholder="WiFi Password" 
                    secureTextEntry={true} 
                    width="100%"
                />
                <AppFormPicker 
                    items={teams}
                    name="team"
                    numberOfColumns={1}
                    PickerItemComponent={TeamPickerComponent}
                    placeholder="Choose a Team"
                    width="100%"
                />
                <AppFormSlider
                    minimumTrackTintColor={colors.primary}
                    minimumValue={0}
                    maximumValue={60}
                    name="delay"
                    step={1}
                    valuePrefix="Goal Light Delay:"
                    valueSuffix="Seconds"
                />
                <SubmitButton title="Submit"/>
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