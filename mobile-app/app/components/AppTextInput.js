import React from 'react';
import { View, StyleSheet, TextInput } from 'react-native';

import { colors, textStyles } from '../config';

function AppTextInput({ width="100%", marginTop=10, ...otherProps }) {
    return (
        <View style={[styles.container, { width, marginTop }]}>
            <TextInput 
                placeholderTextColor={colors.placeholder}
                style={textStyles.default}
                {...otherProps}/>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        backgroundColor: colors.inputBackground,
        borderColor: colors.inputBorder,
        borderWidth: 1,
        borderRadius: 25,
        padding: 10,
        marginVertical: 10,
    },
})

export default AppTextInput;