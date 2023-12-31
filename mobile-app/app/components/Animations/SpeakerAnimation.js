import React from 'react';
import LottieView from 'lottie-react-native';

function SpeakerAnimation({closeIndicator, visible = false}) {
    if (!visible) return null;

    return (
        <LottieView 
            autoPlay={true}
            loop={false}
            onAnimationFinish={closeIndicator}
            source={require('../../assets/animations/loading-bar.json')}
            style={{
                height: 18,
            }}
        />
    );
}

export default SpeakerAnimation;