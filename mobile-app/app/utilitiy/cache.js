import AsyncStorage from '@react-native-async-storage/async-storage';

const storeData = async (value) => {
    try {
        const jsonValue = JSON.stringify(value);
        await AsyncStorage.setItem('default', jsonValue);
    } catch (error) {
        console.log(error);
    }
};

const getData = async () => {
    try {
        const jsonValue = await AsyncStorage.getItem('default');
        return jsonValue != null ? JSON.parse(jsonValue) : null;
    } catch (error) {
        console.log(error);
    }
};

export default { getData, storeData };