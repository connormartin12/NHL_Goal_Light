import React from 'react';
import { Text, StyleSheet, TouchableOpacity } from 'react-native';

function AppButton({color="black", onPress, title}) {
    return (
        <TouchableOpacity style={[styles.button, { backgroundColor: color }]} onPress={onPress}>
            <Text style={styles.text}>{title}</Text>
        </TouchableOpacity>
    );
}

const styles = StyleSheet.create({
    button: {
        alignItems: 'center',
        borderRadius: 25,
        justifyContent: 'center',
        marginVertical: 30,
        padding: 15,
    },
    text: {
        color: 'white',
        fontSize: 18,
        textTransform: 'uppercase',
        fontWeight: 'bold',
    },
})

export default AppButton;