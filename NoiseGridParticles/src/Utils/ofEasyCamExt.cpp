#include "ofEasyCamExt.h"
#include "ofMath.h"
#include "ofUtils.h"

//----------------------------------------
ofEasyCamExt::ofEasyCamExt()
{
	lastTap	= 0;
	lastDistance = 0;
	drag = 0.9f;
	
	sensitivityRot = 0.01f;	//when 1 moving the mouse from one side to the other of the arcball (min(viewport.width, viewport.height)) will rotate 180degrees. when .5, 90 degrees.
	sensitivityXY = 0.5;
	sensitivityZ  = 0.01;
	
	bDistanceSet = false; 
	bMouseInputEnabled = false;
	bDoRotate = false;
	bApplyInertia =false;
	bDoTranslate = false;
	bInsideArcball = true;
	bValidClick = false;
	bEnableMouseMiddleButton = true;
	bAutoDistance = true;
	
	doTranslationKey = 0;
	doDollyKey = 0;
	
	dollyForwardKey = 'a';
	dollyBackwardKey = 'z';
	
	dollyImpulseAmount = 0.01f;
	maxDollyImpulseSpeed = 1.0f;
	
	reset();
	enableMouseInput();	

}

//----------------------------------------
ofEasyCamExt::~ofEasyCamExt(){
	disableMouseInput();
}
//----------------------------------------
void ofEasyCamExt::update(ofEventArgs & args)
{
			
	if(!bDistanceSet && bAutoDistance)
	{
		setDistance(getImagePlaneDistance(viewport), true);
	}
		
	if(bMouseInputEnabled)
	{
		rotationFactor = sensitivityRot * 180 / min(viewport.width, viewport.height);
		if (bMouseInputEnabled)
		{
			updateMouse();
		}
			
		if (bDoRotate)
		{
			updateRotation();
		}
		else if (bDoTranslate)
		{
			updateTranslation(); 
		}
	}
		
	if( dollyForwardKey != 0 )
	{
		if( ofGetKeyPressed(dollyForwardKey) ) { dollyImpulse( -dollyImpulseAmount ); }
	}
		
	if( dollyBackwardKey != 0 )
	{
		if( ofGetKeyPressed(dollyBackwardKey) ) { dollyImpulse( dollyImpulseAmount ); }
	}

	float minDifference = 0.1e-5;
		
//	if (bApplyInertia) {
		moveX *= drag;
		moveY *= drag;
		moveZ *= drag;
		if (ABS(moveX) <= minDifference && ABS(moveY) <= minDifference && ABS(moveZ) <= minDifference) {
			//bApplyInertia = false;
			bDoTranslate = false;
		}
//	}
		
	move((getXAxis() * moveX) + (getYAxis() * moveY) + (getZAxis() * moveZ));
	

}
//----------------------------------------
void ofEasyCamExt::begin(ofRectangle viewport){
	this->viewport = viewport;
	ofCamera::begin(viewport);	
}

//----------------------------------------
void ofEasyCamExt::reset(){
	target.resetTransform();
	
	target.setPosition(0,0, 0);
	lookAt(target);
	
	resetTransform();
	setPosition(0, 0, lastDistance);
	
		
	xRot = 0;
	yRot = 0;
	zRot = 0;
	
	moveX = 0;
	moveY = 0;
	moveZ = 0;
}
//----------------------------------------
void ofEasyCamExt::setTarget(const ofVec3f& targetPoint){
	target.setPosition(targetPoint);
	lookAt(target);
}
//----------------------------------------
void ofEasyCamExt::setTarget(ofNode& targetNode){
	target = targetNode;
	lookAt(target);
}
//----------------------------------------
ofNode& ofEasyCamExt::getTarget(){
	return target;
}
//----------------------------------------
void ofEasyCamExt::setDistance(float distance){
	setDistance(distance, true);
}
//----------------------------------------
void ofEasyCamExt::setDistance(float distance, bool save){//should this be the distance from the camera to the target?
	if (distance > 0.0f){
		if(save){
			this->lastDistance = distance;
		}
		setPosition(target.getPosition() + (distance * getZAxis()));
		bDistanceSet = true;
	}
}
//----------------------------------------
float ofEasyCamExt::getDistance() const {
	return target.getPosition().distance(getPosition());
}
//----------------------------------------
void ofEasyCamExt::setAutoDistance(bool bAutoDistance){
    this->bAutoDistance = bAutoDistance;
    if (bAutoDistance) {
        bDistanceSet = false;
    }
}
//----------------------------------------
void ofEasyCamExt::setDrag(float drag){
	this->drag = drag;
}
//----------------------------------------
float ofEasyCamExt::getDrag() const {
	return drag;
}
//----------------------------------------
void ofEasyCamExt::setTranslationKey(char key){
	doTranslationKey = key;
}
//----------------------------------------
char ofEasyCamExt::getTranslationKey(){
	return doTranslationKey;
}

//----------------------------------------
void ofEasyCamExt::setDollyKey(char _key) {
	doDollyKey = _key;
}

//----------------------------------------
char ofEasyCamExt::getDollyKey() {
	return doDollyKey;
}

//----------------------------------------
void ofEasyCamExt::setDollyForwardKey(char _key)
{
	dollyForwardKey = _key;
}

//----------------------------------------
char ofEasyCamExt::getDollyForwardKey()
{
	return dollyForwardKey;
}

//----------------------------------------
void ofEasyCamExt::setDollyBackwardKey(char _key)
{
	dollyBackwardKey = _key;
}

//----------------------------------------
char ofEasyCamExt::getDollyBackwardKey()
{
	return dollyBackwardKey;
}

//----------------------------------------
void ofEasyCamExt::setDollyImpulseMagnitude( float _impulseMagnitude )
{
	dollyImpulseAmount = _impulseMagnitude;
	maxDollyImpulseSpeed = dollyImpulseAmount * 4.0f;
}

//----------------------------------------
float ofEasyCamExt::getDollyImpulseMagnitude()
{
	return dollyImpulseAmount;
}

//----------------------------------------
void ofEasyCamExt::dollyImpulse( float _impulse ){
	
	//cout << "dollyImpulse: " << _impulse << endl;;
	
	moveZ += _impulse;
	moveZ = ofClamp( moveZ, -maxDollyImpulseSpeed, maxDollyImpulseSpeed );
}

//----------------------------------------
void ofEasyCamExt::enableMouseInput(){
	if(!bMouseInputEnabled){
		bMouseInputEnabled = true;
		ofAddListener(ofEvents().update , this, &ofEasyCamExt::update);
	}
}
//----------------------------------------
void ofEasyCamExt::disableMouseInput(){
	if(bMouseInputEnabled){
		bMouseInputEnabled = false;
		ofRemoveListener(ofEvents().update, this, &ofEasyCamExt::update);
	}
}

//----------------------------------------
bool ofEasyCamExt::getMouseInputEnabled(){
	return bMouseInputEnabled;
}

//----------------------------------------
void ofEasyCamExt::enableMouseMiddleButton(){
	bEnableMouseMiddleButton = true;
}

//----------------------------------------
void ofEasyCamExt::disableMouseMiddleButton(){
	bEnableMouseMiddleButton = false;
}

//----------------------------------------
bool ofEasyCamExt::getMouseMiddleButtonEnabled(){
	return bEnableMouseMiddleButton;
}

//----------------------------------------
void ofEasyCamExt::updateTranslation()
{
	move((getXAxis() * moveX) + (getYAxis() * moveY) + (getZAxis() * moveZ));
}

//----------------------------------------
void ofEasyCamExt::updateRotation(){
	if (bApplyInertia) {
		xRot *=drag; 
		yRot *=drag;
		zRot *=drag;
		
		float minDifference = 0.1e-5;

		if (ABS(xRot) <= minDifference && ABS(yRot) <= minDifference && ABS(zRot) <= minDifference) {
			bApplyInertia = false;
			bDoRotate = false;
		}
	}
	curRot = ofQuaternion(xRot, ofCamera::getXAxis(), yRot, ofCamera::getYAxis(), zRot, ofCamera::getZAxis());
	setPosition((ofCamera::getGlobalPosition()-target.getGlobalPosition())*curRot +target.getGlobalPosition());
	rotate(curRot);
}
//----------------------------------------
void ofEasyCamExt::updateMouse(){
		
	
	mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
	
	if(viewport.inside(mouse.x, mouse.y) && !bValidClick && (ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT) || ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE)) )
	{
		unsigned long doubleclickTime = 200;
		unsigned long curTap = ofGetElapsedTimeMillis();
		if(lastTap != 0 && curTap - lastTap < doubleclickTime)
		{
			reset();
		}
		
		if ((bEnableMouseMiddleButton && ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE)) ||
			ofGetKeyPressed(doTranslationKey) ||
			ofGetKeyPressed(doDollyKey) /*|| ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT)*/ )
		{
			bDoTranslate = true;
			bDoRotate = false;
			bApplyInertia = false;
		}
		else if (ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT) )
		{			
			bDoTranslate = false;
			bDoRotate = true;
			bApplyInertia = false;
			if(ofVec2f(mouse.x - viewport.x - (viewport.width/2), mouse.y - viewport.y - (viewport.height/2)).length() < min(viewport.width/2, viewport.height/2))
			{
				bInsideArcball = true;
			}
			else
			{
				bInsideArcball = false;
			}
		}
		
		lastTap = curTap;
		//lastMouse = ofVec2f(ofGetPreviousMouseX(),ofGetPreviousMouseY()); //this was causing the camera to have a tiny "random" rotation when clicked.
		lastMouse = mouse;
		bValidClick = true;
		bApplyInertia = false;
	}
	
	
	if (bValidClick)
	{
		if ( !(ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT) || ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE)) )
		{
			bApplyInertia = true;
			bValidClick = false;
		}
		else
		{
			int vFlip;
			if(isVFlipped()){
				vFlip = -1;
			}else{
				vFlip =  1;
			}
			
			mouseVel = mouse  - lastMouse;
			
			if (bDoTranslate)
			{
				moveX = 0;
				moveY = 0;
				//moveZ = 0;
				
				//if ( ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT))
				if ( ofGetKeyPressed(doDollyKey) )
				{
					//moveZ = mouseVel.y * sensitivityZ * (getDistance() + FLT_EPSILON)/ viewport.height;
					moveZ += mouseVel.y * sensitivityZ * (getDistance() + FLT_EPSILON)/ viewport.height;
				}
				else
				{
					moveX = -mouseVel.x * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.width;
					moveY = vFlip * mouseVel.y * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.height;
				}
			}
			else
			{
				//xRot = 0;
				//yRot = 0;
				//zRot = 0;
				
				if (bInsideArcball)
				{
					//xRot = vFlip * -mouseVel.y * rotationFactor;
					//yRot = -mouseVel.x * rotationFactor;
					
					xRot += vFlip * -mouseVel.y * rotationFactor;
					yRot += -mouseVel.x * rotationFactor;
				}
				else
				{
					ofVec2f center(viewport.width/2, viewport.height/2);
					//zRot = -vFlip * ofVec2f(mouse.x - viewport.x - center.x, mouse.y - viewport.y - center.y).angle(lastMouse - ofVec2f(viewport.x, viewport.y) - center);
					zRot += (-vFlip * ofVec2f(mouse.x - viewport.x - center.x, mouse.y - viewport.y - center.y).angle(lastMouse - ofVec2f(viewport.x, viewport.y) - center)) * sensitivityRot;
				}
			}
			lastMouse = mouse;
		}
	}

}


