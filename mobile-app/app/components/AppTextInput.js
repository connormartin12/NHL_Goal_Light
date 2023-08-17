import React from 'react';
import { View, StyleSheet, TextInput, TouchableOpacity } from 'react-native';
import { MaterialCommunityIcons } from '@expo/vector-icons';

import { colors, textStyles } from '../config';

function AppTextInput({ width="100%", marginTop=10, onPress, passwordField, ...otherProps }) {
    return (
        <View style={[styles.container, { width, marginTop }]}>
            <TextInput 
                placeholderTextColor={colors.placeholder}
                style={textStyles.default}
                width="75%"
                {...otherProps}/>
            <TouchableOpacity disabled={otherProps.readOnly} onPress={onPress} style={styles.icon}>
                { passwordField ? <MaterialCommunityIcons 
                    name="eye"
                    size={25}
                    color={colors.icon}
                /> : <></>}
               
            </TouchableOpacity>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        backgroundColor: colors.inputBackground,
        borderColor: colors.inputBorder,
        borderWidth: 1,
        borderRadius: 25,
        flexDirection: "row",
        padding: 10,
        marginVertical: 10,
    },
    icon: {
        marginLeft: 'auto',
        marginRight: '5%',
    },
})

export default AppTextInput;