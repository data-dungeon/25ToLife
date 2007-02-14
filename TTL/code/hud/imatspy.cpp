///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/ttlweapinv.h"
#include "weapons/ttlweapon.h"
#include "hud/imatspy.h"

#if !defined(CDROM) && !defined(DIRECTX_PC)
bool MaterialSpyInstrument::s_enable = true;
#else
bool MaterialSpyInstrument::s_enable = false;
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MaterialSpyInstrument::MaterialSpyInstrument() :
TextInstrument("matspy")
{
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void MaterialSpyInstrument::Update(float dt)
{
	// Wee!
	if (!s_enable)
		SetText(NULL);
	else
	{
		CCompActor *actor = g_hud.GetSubject();
		TTLWeaponInventory *inventory = actor ? (TTLWeaponInventory *)actor->GetComponentByName("WeaponInventory") : NULL;
		TTLWeapon *weapon = inventory ? (TTLWeapon *)inventory->GetSelected() : NULL;
		if (!actor || !weapon)
			SetText(NULL);
		else
		{
			int materials;
			int *material = weapon->GetAimSim().TargetMaterials(materials);
			if (!material || materials == 0)
				SetText(NULL);
			else
			{
				bool haveDefault = false;
				char text[128];
				text[0] = '\0';
				for (int m = 0; m < materials; m++)
				{
					haveDefault = haveDefault || (material[m] == 0);
					char *name = g_surfaceType.GetName(material[m]);
					if (name)
					{
						strcat(text, name);
						if (m != materials - 1)
							strcat(text, " | ");
					}
				}
				SetText(text);
				if (haveDefault)
					m_text.SetColor(1.0f, 0.5f, 0.5f, 0.75f);
				else
					m_text.SetColor(0.5f, 0.5f, 0.5f, 0.75f);
				m_text.SetPosition(m_pos.X(), m_pos.Y() + 32.0f);
			}
		}
	}

	// Do the rest of the wrok
	TextInstrument::Update(dt);
}

