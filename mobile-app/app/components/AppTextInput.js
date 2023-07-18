import React from 'react';
import { View, StyleSheet, TextInput } from 'react-native';

import textStyles from '../config/textStyles';

function AppTextInput({ placeholder, ...otherProps }) {
    return (
        <View style={styles.container}>
            <TextInput 
                placeholder={placeholder}
                placeholderTextColor="#636969"
                style={textStyles.default}
                {...otherProps} />
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        backgroundColor: "#F5F5F5",
        borderColor: "#a9a9a9",
        borderWidth: 1,
        borderRadius: 25,
        padding: 10,
        marginVertical: 10,
    },
})

export default AppTextInput;