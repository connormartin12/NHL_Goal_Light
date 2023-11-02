import React from 'react';
import { Pressable, StyleSheet } from 'react-native';

import AppText from './AppText';

function AppLink({ disabled, onPress, style, linkText }) {
    return (
        <Pressable disabled={disabled} style={style} onPress={onPress}>
            <AppText style={styles.text}>{linkText}</AppText>
        </Pressable>
    );
}

const styles = StyleSheet.create({
    text: {
        fontSize: 20,
        fontWeight: '500',
        textDecorationLine: "underline",
    }
})

export default AppLink;