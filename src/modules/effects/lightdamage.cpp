#include "lightdamage.h"
#include <modules/gfx/packet.h>

namespace MM2
{
	mmLightDamage::mmLightDamage()
	{
		*Model = nullptr;
		*LightModel = nullptr;
		*VertexPositions = nullptr;
		*VertexCount = 0;
	}

	mmLightDamage::~mmLightDamage()
	{
		if (*VertexCount)
		{
			delete[] * VertexPositions;
			*VertexPositions = nullptr;

			*VertexCount = 0;
		}
	}

	void mmLightDamage::Init(modStatic* model, int id)
	{
		if (model == nullptr)
			return;

		Model[id] = model;
		LightModel[id] = model->Clone();

		int adjunctCount = model->GetAdjunctCount();

		VertexCount[id] = adjunctCount;

		VertexPositions[id] = new Vector3[adjunctCount];

		int currentVertex = 0;

		for (int i = 0; i < model->GetPacketCount(); i++)
		{
			auto packet = model->GetPacket(i);
			while (packet)
			{
				packet->DoLock();

				for (unsigned int j = 0; j < packet->GetAdjunctCount(); j++)
				{
					packet->GetPosition(VertexPositions[id][currentVertex], j);
					currentVertex++;
				}

				packet->DoUnlock();
				packet = packet->GetNext();
			}
		}
	}

	void mmLightDamage::Reset(bool showDamage)
	{
		for (int i = 0; i < mmLightDamage::LightCount; i++)
		{
			if (Model[i] == nullptr && LightModel[i] == nullptr)
				continue;

			for (int j = 0; j < Model[i]->GetPacketCount(); j++)
			{
				auto modelPacket = Model[i]->GetPacket(j);
				auto brokenPacket = LightModel[i]->GetPacket(j);

				while (modelPacket && brokenPacket)
				{
					for (unsigned int k = 0; k < modelPacket->GetTriangleCount() / 3; k++)
					{
						int tri[3];
						modelPacket->GetTri(tri, k);

						if (showDamage)
						{
							brokenPacket->SetTri(0, 0, 0, k);
						}
						else
						{
							brokenPacket->SetTri(tri[0], tri[1], tri[2], k);
						}
					}

					modelPacket = modelPacket->GetNext();
					brokenPacket = brokenPacket->GetNext();
				}
			}
		}
	}

	void mmLightDamage::Apply(Vector3& impactPosition, float radius)
	{
		for (int i = 0; i < mmLightDamage::LightCount; i++)
		{
			if (Model[i] == nullptr && LightModel[i] == nullptr)
				continue;

			int currentVertex = 0;

			for (int j = 0; j < Model[i]->GetPacketCount(); j++)
			{
				auto modelPacket = Model[i]->GetPacket(j);
				auto brokenPacket = LightModel[i]->GetPacket(j);

				while (modelPacket && brokenPacket)
				{
					for (unsigned int k = 0; k < modelPacket->GetTriangleCount() / 3; k++)
					{
						int tri[3];
						modelPacket->GetTri(tri, k);

						float dist0 = (VertexPositions[i][currentVertex + tri[0]] - impactPosition).Mag2();
						float dist1 = (VertexPositions[i][currentVertex + tri[1]] - impactPosition).Mag2();
						float dist2 = (VertexPositions[i][currentVertex + tri[2]] - impactPosition).Mag2();

						if (dist0 < radius || dist1 < radius || dist2 < radius)
						{
							brokenPacket->SetTri(0, 0, 0, k);
						}
					}
					currentVertex += brokenPacket->GetAdjunctCount();

					modelPacket = modelPacket->GetNext();
					brokenPacket = brokenPacket->GetNext();
				}
			}
		}
	}

	modStatic* mmLightDamage::GetLightModel(int id) const
	{
		return this->LightModel[id];
	}
}