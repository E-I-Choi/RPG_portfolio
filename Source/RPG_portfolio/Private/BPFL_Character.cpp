#include "BPFL_Character.h"

FStatus UBPFL_Character::GetBaseStatusByClass(EClassType InClassType)
{
	FStatus Stats;
	switch (InClassType)
	{
	default: break;
	case EClassType::none:
		Stats.Strength = 5;
		Stats.Health = 5;
		Stats.Agility = 5;
		Stats.Intelligence = 5;
		break;
	case EClassType::mage:
		Stats.Strength = 5;
		Stats.Health = 5;
		Stats.Agility = 6;
		Stats.Intelligence = 15;
		break;
	case EClassType::archer:
		Stats.Strength = 8;
		Stats.Health = 6;
		Stats.Agility = 10;
		Stats.Intelligence = 7;
		break;
	case EClassType::assassin:
		Stats.Strength = 6;
		Stats.Health = 5;
		Stats.Agility = 12;
		Stats.Intelligence = 8;
		break;
	case EClassType::warrior:
		Stats.Strength = 11;
		Stats.Health = 9;
		Stats.Agility = 6;
		Stats.Intelligence = 5;
		break;
	}
	return Stats;
}

FEquips UBPFL_Character::GetBaseEquipsByClass(EClassType InClassType)
{
	FEquips Equips;
	switch (InClassType)
	{
	default: break;

	case EClassType::none:
		Equips.Weapon = EWeaponType::cudgel;
		Equips.Hat = EHatType::none;
		Equips.Top = ETopType::cotton_top;
		Equips.Pants = EBottomsType::cotton_bottoms;
		Equips.Shoes = EFootsType::flat_shoes;
		break;
	case EClassType::archer:
		Equips.Weapon = EWeaponType::bow;
		Equips.Hat = EHatType::feather_hat;
		Equips.Top = ETopType::cotton_top;
		Equips.Pants = EBottomsType::cotton_bottoms;
		Equips.Shoes = EFootsType::combat_shoes;
		break;
	case EClassType::assassin:
		Equips.Weapon = EWeaponType::dagger;
		Equips.Hat = EHatType::hood;
		Equips.Top = ETopType::cotton_top;
		Equips.Pants = EBottomsType::cotton_bottoms;
		Equips.Shoes = EFootsType::flat_shoes;
		break;
	case EClassType::mage:
		Equips.Weapon = EWeaponType::wand;
		Equips.Hat = EHatType::witch_hat;
		Equips.Top = ETopType::robe;
		Equips.Pants = EBottomsType::cotton_bottoms;
		Equips.Shoes = EFootsType::flat_shoes;
		break;
	case EClassType::warrior:
		Equips.Weapon = EWeaponType::sword;
		Equips.Hat = EHatType::helmet;
		Equips.Top = ETopType::armor_top;
		Equips.Pants = EBottomsType::armor_bottoms;
		Equips.Shoes = EFootsType::combat_shoes;
		break;
	}
	return Equips;
}
