import React, { useEffect, useRef } from 'react';
import LottieView from 'lottie-react-native';

function ConnectingIndicator({closeIndicator, visible = false}) {
    if (!visible) return null;

    const animationRef = useRef(null);

    useEffect(() => {
        animationRef.current?.play(0, 35);
    }, []);

    return (
        <LottieView
            loop={false}
            onAnimationFinish={closeIndicator}
            ref={animationRef}
            source={require('../../assets/animations/loading-circle.json')}
            style={{
                height: 50,
            }}
        />
    );
}

export default ConnectingIndicator;