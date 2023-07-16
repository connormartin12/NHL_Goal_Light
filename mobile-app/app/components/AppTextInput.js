import React from 'react';
import { View, StyleSheet, TextInput } from 'react-native';

function AppTextInput({ placeholder, ...otherProps }) {
    return (
        <View style={styles.container}>
            <TextInput 
                placeholder={placeholder}
                placeholderTextColor="#636969"
                style={styles.text}
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
    text: {
        fontSize: 18,
        fontFamily: "Avenir",
        color: "black",
    },
})

export default AppTextInput;