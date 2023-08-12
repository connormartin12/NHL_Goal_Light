import React from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';

import UserInputScreen from './app/screens/UserInputScreen';
import HomeScreen from './app/screens/HomeScreen';

const Stack = createNativeStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Connection" component={HomeScreen}  options={{ headerShown: false }} />
        <Stack.Screen name="User Input" component={UserInputScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}