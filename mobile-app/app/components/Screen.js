import React from 'react';
import Constants from 'expo-constants';
import {StyleSheet, SafeAreaView, View} from 'react-native';

function Screen({ children, style }) {
    return (
        <SafeAreaView style={[styles.container, style]}>
            <View style={[styles.view, style]}>{children}</View>
        </SafeAreaView>
    );
}

const styles = StyleSheet.create({
    screen: {
        paddingTop: Constants.statusBarHeight,
        flex: 1,
    },
    view: {
        flex: 1,
    },
})

export default Screen;