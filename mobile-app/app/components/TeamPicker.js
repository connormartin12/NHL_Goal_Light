import React, { useState } from 'react';
import { Button, FlatList, Modal, StyleSheet, TouchableOpacity, TouchableWithoutFeedback, View } from 'react-native';
import { MaterialCommunityIcons } from '@expo/vector-icons';

import AppText from './AppText';
import Screen from './Screen';

const teams = [
    { name: "Anaheim Ducks", id: 24 },
    { name: "Arizona Coyotes", id: 53 },
    { name: "Boston Bruins", id: 6 },
    { name: "Buffalo Sabres", id: 7 },
    { name: "Calgary Flames", id: 20 },
    { name: "Carolina Hurricanes", id: 12 },
    { name: "Chicago Blackhawks", id: 16 },
    { name: "Colorado Avalanche", id: 21 },
    { name: "Columbus Blue Jackets", id: 29 },
    { name: "Dallas Stars", id: 25 },
    { name: "Detroit Red Wings", id: 17 },
    { name: "Edmonton Oilers", id: 22 },
    { name: "Florida Panthers", id: 13 },
    { name: "Los Angeles Kings", id: 26 },
    { name: "Minnesota Wild", id: 30 },
    { name: "Montreal Canadiens", id:8 },
    { name: "Nashville Predators", id: 18, },
    { name: "New Jersey Devils", id: 1 },
    { name: "New York Islanders", id: 2 },
    { name: "New York Rangers", id: 3 },
    { name: "Ottawa Senators", id: 9 },
    { name: "Philadelphia Flyers", id: 4 },
    { name: "Pittsburgh Penguins", id: 5 },
    { name: "San Jose Sharks", id: 28 },
    { name: "Seattle Kraken", id: 55 },
    { name: "St. Louis Blues", id: 19 }, 
    { name: "Tampa Bay Lightning", id: 14 },
    { name: "Toronto Maple Leafs", id: 10 },
    { name: "Vancouver Canucks", id: 23 },
    { name: "Vegas Golden Knights", id: 54 },
    { name: "Washington Capitals", id: 15 },
    { name: "Winnipeg Jets", id: 52 },
];

function TeamPicker() {
    const [modalVisible, setModalVisible] = useState(false);
    const [selectedItem, setSelectedItem] = useState("");

    const Item = ({name, onPress}) => (
        <TouchableOpacity onPress={onPress} style={styles.listItem}>
            <AppText style={styles.listItemText}>{name}</AppText>
        </TouchableOpacity>
    );

    return (
        <>
            <TouchableWithoutFeedback onPress={() => setModalVisible(true)}>
                <View style={styles.container}>
                    { (selectedItem != "") ? (<AppText>{selectedItem}</AppText>) : (<AppText style={styles.placeholder}>Choose a Team</AppText>)
}
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
                        data={teams}
                        renderItem={({ item }) => (
                            <Item 
                                name={item.name}
                                onPress={() => {
                                    setModalVisible(false);
                                    setSelectedItem(item.name);
                                }}    
                            />
                        )}
                        keyExtractor={item => item.id}
                    />
                </Screen>
            </Modal>
        </>
    );
};

const styles = StyleSheet.create({
    container: {
        alignItems: 'center',
        backgroundColor: '#F5F5F5',
        borderColor: '#a9a9a9',
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
        color: "#636969",
    },
    separator: {
        backgroundColor: '#DEDEDE',
        height: 1,
        marginLeft: '2.5%',
        marginRight: '2.5%',
    },
});

export default TeamPicker;