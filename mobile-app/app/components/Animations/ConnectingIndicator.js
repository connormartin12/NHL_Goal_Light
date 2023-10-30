import React from 'react';
import LottieView from 'lottie-react-native';

function ConnectingIndicator({closeIndicator, visible = false}) {
    if (!visible) return null;

    return (
        <LottieView 
            autoPlay={true}
            loop={false}
            onAnimationFinish={closeIndicator}
            source={require('../../assets/animations/loading-circle.json')}
            style={{
                height: 50,
            }}
        />
    );
}

export default ConnectingIndicator;