import React from 'react';
import { StyleSheet } from 'react-native';
import { Formik } from 'formik';
import * as Yup from 'yup';

import AppFormField from '../components/AppFormField';
import Screen from '../components/Screen';
import SubmitButton from '../components/SubmitButton';

let validationSchema = Yup.object({
    ssid: Yup.string().required().min(1).max(32).label("SSID"),
    password: Yup.string().required().min(1).max(63).label(" WiFi Password")
});

function UserInputScreen() {
    const handleSubmit = (userData, { resetForm }) => {
        console.log("SSID:", userData.ssid);
        console.log("Password:", userData.password);
        resetForm();
    } 

    return (
        <Screen style={styles.container}>
            <Formik
                initialValues={{ ssid: '', password: ''}}
                onSubmit={handleSubmit}
                validationSchema={validationSchema}
            >
                <>
                    <AppFormField 
                        autoCapitalize="none"
                        autoCorrect={false}
                        name="ssid"
                        placeholder="WiFi SSID" 
                    />
                    <AppFormField
                        autoCapitalizee="none"
                        autoCorrect={false}
                        name="password"
                        placeholder="WiFi Password" 
                        secureTextEntry={true} 
                        // textContentType="password" 
                    />
                    <SubmitButton title="Submit"/>
                </>
            </Formik>
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