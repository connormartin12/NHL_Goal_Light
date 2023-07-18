import { Platform } from 'react-native';

export default {
    default: {
        fontSize: 18,
        fontFamily: Platform.OS === "ios" ? "Avenir" : "Roboto",
        color: "black",
    },
};