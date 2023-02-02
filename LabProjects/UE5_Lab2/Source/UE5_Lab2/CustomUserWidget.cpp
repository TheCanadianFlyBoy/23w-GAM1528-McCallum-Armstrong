// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUserWidget.h"
#include "Components/TextBlock.h"
#include "UE5_Lab2Character.h"

bool UCustomUserWidget::Initialize()
{
    bool bResult = Super::Initialize();

    return false;
}

void UCustomUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    DisplayGrenadeCount();
    
}

void UCustomUserWidget::DisplayGrenadeCount()
{
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("GrenadeTextBlock"));
    AUE5_Lab2Character* Player = Cast<AUE5_Lab2Character>(GetOwningPlayerPawn());

    if (Widget && Player)
    {
        Widget->SetText(FText::FromString("Grenades: " + FString::FromInt(Player->GetGrenadeCount())));
    }
}
