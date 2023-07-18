import React from 'react';
import {StyleSheet, SafeAreaView, StatusBar, View} from 'react-native';

function Screen({ children, style }) {
    return (
        <SafeAreaView style={[styles.screen, style]}>
            <View style={[styles.view, style]}>{children}</View>
        </SafeAreaView>
    );
}

const styles = StyleSheet.create({
    screen: {
        marginTop: StatusBar.currentHeight || 0,
        flex: 1,
    },
    view: {
        flex: 1,
    },
})

export default Screen;