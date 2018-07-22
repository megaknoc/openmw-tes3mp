#include "aiactivate.hpp"

#include <components/esm/aisequence.hpp>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"

#include "../mwworld/class.hpp"

#include "creaturestats.hpp"
#include "steering.hpp"
#include "movement.hpp"

namespace MWMechanics
{
    AiActivate::AiActivate(const std::string &objectId)
        : mObjectId(objectId)
    {
    }

    /*
        Start of tes3mp addition

        Allow AiActivate to be initialized using a Ptr instead of a refId
    */
    AiActivate::AiActivate(MWWorld::Ptr object)
        : mObjectId("")
    {
        mObjectPtr = object;
    }
    /*
        End of tes3mp addition
    */

    AiActivate *MWMechanics::AiActivate::clone() const
    {
        return new AiActivate(*this);
    }

    bool AiActivate::execute(const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
    {
        /*
            Start of tes3mp change (major)

            Only search for an object based on its refId if we haven't provided a specific object already
        */
        const MWWorld::Ptr target = mObjectId.empty() ? mObjectPtr : MWBase::Environment::get().getWorld()->searchPtr(mObjectId, false);
        /*
            End of tes3mp change (major)
        */

        actor.getClass().getCreatureStats(actor).setDrawState(DrawState_Nothing);

        if (target == MWWorld::Ptr() ||
            !target.getRefData().getCount() || !target.getRefData().isEnabled()  // Really we should check whether the target is currently registered
                                                                                // with the MechanicsManager
            )
        return true;   //Target doesn't exist

        //Set the target destination for the actor
        ESM::Pathgrid::Point dest = target.getRefData().getPosition().pos;

        if (pathTo(actor, dest, duration, MWBase::Environment::get().getWorld()->getMaxActivationDistance())) //Stop when you get in activation range
        {
            // activate when reached

            /*
                Start of tes3mp change (major)

                Disable unilateral activation on this client and expect the server's reply to our
                packet to do it instead
            */
            //MWBase::Environment::get().getWorld()->activate(target, actor);
            /*
                End of tes3mp change (major)
            */

            /*
                Start of tes3mp addition

                Send an ID_OBJECT_ACTIVATE packet every time an object is activated here
            */
            mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
            objectList->reset();
            objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
            objectList->addObjectActivate(target, actor);
            objectList->sendObjectActivate();
            /*
                End of tes3mp addition
            */

            return true;
        }

        return false;
    }

    int AiActivate::getTypeId() const
    {
        return TypeIdActivate;
    }

    void AiActivate::writeState(ESM::AiSequence::AiSequence &sequence) const
    {
        std::unique_ptr<ESM::AiSequence::AiActivate> activate(new ESM::AiSequence::AiActivate());
        activate->mTargetId = mObjectId;

        ESM::AiSequence::AiPackageContainer package;
        package.mType = ESM::AiSequence::Ai_Activate;
        package.mPackage = activate.release();
        sequence.mPackages.push_back(package);
    }

    AiActivate::AiActivate(const ESM::AiSequence::AiActivate *activate)
        : mObjectId(activate->mTargetId)
    {
    }
}
