// Copyright 2024 TabbyCoder

#include "Helpers.h"

void WriteOnScreen(const FString& Text) {
  #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    if (GEngine != nullptr) {
      GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, Text);
    }
    UE_LOG(LogTemp, Error, TEXT("%s"), *Text); return;
  #else // UE_BUILD_SHIPPING
  #endif
  }
  
  void WriteOnScreen(const TCHAR* TextFormat, ...) {
  #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    TCHAR Buffer[1024];
  
    va_list Args;
    va_start(Args, TextFormat);
    FCString::GetVarArgs(Buffer, UE_ARRAY_COUNT(Buffer), TextFormat, Args);
    va_end(Args);
    
    if (GEngine != nullptr) {
      GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString(Buffer));
    }
    UE_LOG(LogTemp, Error, TEXT("%s"), Buffer); return;
  #else // UE_BUILD_SHIPPING
  #endif
  }
  
  void ReportError(const char* FunctionName, const char* TextFormat, ...) {
    // For now, write on screen with a different color.
    // Maybe we should write to some logs also?
  #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    FString WideFunctionName = UTF8_TO_TCHAR(FunctionName);
    FString WideTextFormat = UTF8_TO_TCHAR(TextFormat);
    const TCHAR* WideTextFormatPtr = *WideTextFormat;
  
    TCHAR UserBuffer[1024];
    va_list Args;
    va_start(Args, TextFormat);
    FCString::GetVarArgs(UserBuffer, UE_ARRAY_COUNT(UserBuffer), WideTextFormatPtr, Args);
    va_end(Args);
  
    TCHAR PrefixedBuffer[1300];
    FCString::Snprintf(PrefixedBuffer, UE_ARRAY_COUNT(PrefixedBuffer), TEXT("[%s] %s."), *WideFunctionName, UserBuffer);
  
    if (GEngine != nullptr) {
      GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString(PrefixedBuffer));
    }
    UE_LOG(LogTemp, Error, TEXT("%s"), PrefixedBuffer); return;
  #else // UE_BUILD_SHIPPING
  #endif
  }
  
  