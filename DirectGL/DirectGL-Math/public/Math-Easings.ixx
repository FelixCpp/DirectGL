// Project Name : DirectGL-Math
// File Name    : Math-Easings.ixx
// Author       : Felix Busch
// Created Date : 2025/10/12

export module DirectGL.Math:Easings;

export namespace DGL::Math::Easings
{
	float EaseInSine(float x);		float EaseOutSine(float x);		float EaseInOutSine(float x);
	float EaseInCubic(float x);		float EaseOutCubic(float x);	float EaseInOutCubic(float x);
	float EaseInQuint(float x);		float EaseOutQuint(float x);	float EaseInOutQuint(float x);
	float EaseInCirc(float x);		float EaseOutCirc(float x);		float EaseInOutCirc(float x);
	float EaseInElastic(float x);	float EaseOutElastic(float x);	float EaseInOutElastic(float x);
	float EaseInQuad(float x);		float EaseOutQuad(float x);		float EaseInOutQuad(float x);
	float EaseInQuart(float x);		float EaseOutQuart(float x);	float EaseInOutQuart(float x);
	float EaseInExpo(float x);		float EaseOutExpo(float x);		float EaseInOutExpo(float x);
	float EaseInBack(float x);		float EaseOutBack(float x);		float EaseInOutBack(float x);
	float EaseInBounce(float x);	float EaseOutBounce(float x);	float EaseInOutBounce(float x);
}