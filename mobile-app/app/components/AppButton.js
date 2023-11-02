import React from 'react';
import { Text, StyleSheet, TouchableOpacity } from 'react-native';

import { colors } from '../config';

function AppButton({color=colors.primary, onPress, title, width='100%', ...otherprops}) {
    return (
        <TouchableOpacity style={[styles.button, { backgroundColor: color, width: width }]} onPress={onPress} {...otherprops}>
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
        color: colors.secondary,
        fontSize: 18,
        textTransform: 'uppercase',
        fontWeight: 'bold',
    },
})

export default AppButton;