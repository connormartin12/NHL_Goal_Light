import React, { useState } from 'react';
import { View, StyleSheet, TouchableWithoutFeedback, Button, Modal, FlatList } from 'react-native';
import { MaterialCommunityIcons } from  '@expo/vector-icons';

import AppText from './AppText';
import colors from '../config/colors';
import Screen from './Screen';
import TeamPickerComponent from './TeamPickerComponent';

function AppPicker({ items, numberOfColumns=1, onSelectItem, PickerItemComponent=TeamPickerComponent, placeholder, selectedItem, width="100%" }) {
    const [modalVisible, setModalVisible] = useState(false);

    return (
        <>
            <TouchableWithoutFeedback onPress={() => setModalVisible(true)} >
                <View style={[styles.container, { width }]}>
                    { selectedItem ? (<AppText style={styles.text}>{selectedItem.name}</AppText>) : (<AppText style={styles.placeholder}>{placeholder}</AppText>) }

                    <MaterialCommunityIcons 
                        name="chevron-down" 
                        size={25}
                        style={styles.icon} 
                        color="#636969"
                        />
                </View>
            </TouchableWithoutFeedback>
            <Modal 
                visible={modalVisible} 
                animationType="slide">
                <Screen style={styles.modalView}>
                    <Button title="Close" onPress={() => setModalVisible(false)} />
                        <FlatList
                            ItemSeparatorComponent={
                            <View style={styles.separator} />
                            } 
                            data={items}
                            keyExtractor={item => item.id}
                            numColumns={numberOfColumns}
                            renderItem={({ item }) => (
                                <PickerItemComponent 
                                    item={item}
                                    name={item.name}
                                    onPress={() => {
                                        setModalVisible(false);
                                        onSelectItem(item);
                                    }}
                                />
                             )}
                        />
                </Screen>
            </Modal>
        </>
    )
}

const styles = StyleSheet.create({
    container: {
        alignItems: 'center',
        backgroundColor: colors.inputBackground,
        borderColor: colors.inputBorder,
        borderWidth: 1,
        borderRadius: 25,
        flexDirection: "row",
        padding: 10,
        marginVertical: 10,
    },
    icon: {
        flexDirection: 'row',
        marginLeft: '90%',
        position: 'absolute',
    },
    listItem: {
        width: '100%',
    },
    listItemText: {
        padding: 10,
    },
    modalView: {
        flex: 1,
    },
    placeholder: {
        color: colors.placeholder,
    },
    separator: {
        backgroundColor: '#DEDEDE',
        height: 1,
        marginLeft: '2.5%',
        marginRight: '2.5%',
    },
})

export default AppPicker;