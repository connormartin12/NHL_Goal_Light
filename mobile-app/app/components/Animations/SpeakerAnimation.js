import React from 'react';
import LottieView from 'lottie-react-native';

function SpeakerAnimation({closeSpeakerIndicator, visible = false}) {
    if (!visible) return null;

    return (
        <LottieView 
            autoPlay={true}
            loop={false}
            onAnimationFinish={closeSpeakerIndicator}
            source={require('../../assets/animations/loading-bar.json')}
            style={{
                height: 18,
            }}
        />
    );
}

export default SpeakerAnimation;