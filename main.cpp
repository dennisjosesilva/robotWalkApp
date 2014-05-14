#include <Ogre.h>
#include <ExampleApplication.h>
class RobotWalkListener:public ExampleFrameListener, /*public OIS::KeyListener*/
                               public OIS::MouseListener
{
public:
    RobotWalkListener(RenderWindow *win, Camera *cam,SceneManager *sceneMgr, Entity *ent)
        :ExampleFrameListener(win, cam, true, true)
    {
        //Velocidade do Robo
      //  mMove = 5.0;
        mWalkSpeed = 80.0;

        //Velocidade da Rotação da Camera
        mRotate = 0.25;

        //Robo
        mEntity = ent;

        //Inicialização do teclado
        mMouse->setEventCallback(this);

        //Animação Inicial
        mAnimationState = mEntity->getAnimationState("Idle");
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);

        //Ponto para direcionar o robo
        mDirection = Vector3::ZERO;

        //Variavel para controlar o fim da aplicação
        mContinue = true;
    }

    bool frameStarted(const FrameEvent &evt)
    {
        //Entrada pelo teclado
        if(mKeyboard)
           mKeyboard->capture();

        if(mMouse)
           mMouse->capture();

        //Inicia Variavel para controle de velocidade
        Real move = mWalkSpeed * evt.timeSinceLastFrame;

        //Anima a Cena
        mAnimationState->addTime(evt.timeSinceLastFrame);

        //Move o Robo
        SceneNode *node = mEntity->getParentSceneNode();
        if(!keyDown())
        {
            mDirection = Vector3::ZERO;

            mAnimationState = mEntity->getAnimationState("Idle");
            mAnimationState->setLoop(true);
            mAnimationState->setEnabled(true);
        }
        node->translate(mDirection * move);
        return mContinue;
    }


    bool mouseMoved(const OIS::MouseEvent &e)
    {
       SceneNode *node = mEntity->getParentSceneNode();
       node->yaw(Degree(-mRotate * e.state.X.rel), Node::TS_WORLD);
       mCamera->getParentSceneNode()->pitch(Degree(-mRotate * e.state.Y.rel), Node::TS_LOCAL);

       return true;
    }

    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool keyDown()
    {

        //Controla a entrada dos botões quando são pressionados
       if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
         mContinue = false;


       if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W))
       {
         mDirection = mCamera->getRealDirection();
         mDirection.y = 0;
         animationRobot();
         return true;
       }

       if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S))
       {
          mDirection = -mCamera->getRealDirection();
          mDirection.y = 0;
          animationRobot();
          return true;
       }

       if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A))
       {
          mDirection = -mCamera->getRealRight();
          mDirection.y = 0;
          animationRobot();
          return true;
       }

       if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D))
       {
          mDirection = mCamera->getRealRight();
          mDirection.y = 0;
          animationRobot();
          return true;
       }

        return false;

        mAnimationState = mEntity->getAnimationState("Walk");
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);


    }

    void animationRobot()
    {
        mAnimationState = mEntity->getAnimationState("Walk");
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
    }

    bool keyReleased()
    {
       //Controla o robo quando os botões são soltos

    }

protected:
 //  Real mMove;
   Real mRotate;
   Real mWalkSpeed;

   Entity *mEntity;
   AnimationState *mAnimationState;

   bool mContinue;
   Vector3 mDirection;
};

class SampleApp : public ExampleApplication
{
public:
    SampleApp()
    {}

protected:
    Entity *mEntity;
    SceneNode *mSceneNode;

    void chooseSceneManager()
    {
        //Cria um SceneManager para renderizar terreno
        mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE,"TerrainSceneManager");
    }

    void createScene(void)
    {
        //Cria a Cena estatica
        mSceneMgr->setAmbientLight(ColourValue(1,1,1));
        mSceneMgr->setSkyDome(true, "Examples/CloudySky", 7, 8);

        //Declara o plano com o vetor normal e distancia da origem
        Plane plane(Vector3::UNIT_Y,0);

        //Cria um plano de 1500 por 1500 pixels
        MeshManager::getSingleton().createPlane("ground",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
        1500,1500,20,20,true,1,5,5, Vector3::UNIT_Z);

        //aproxima o plano do Nó de cena Raiz
        Entity *ent = mSceneMgr->createEntity("GroundEntity", "ground");
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

        //Troca o material
        ent->setMaterialName("Examples/Rockwall");
        ent->setCastShadows(false);


        //Cria o robo e o aproxima de um no para ser renderizado
        mEntity = mSceneMgr->createEntity("Robot","robot.mesh");
        mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RobotNode");
        mSceneNode->attachObject(mEntity);

        //Coloca a camera para seguir o robo
        mSceneNode->createChildSceneNode("CamNode")->attachObject(mCamera);

        //Coloca a Camera em uma visão melhor
        mCamera->getParentSceneNode()->setPosition(Vector3(50,120, 0));
        mCamera->getParentSceneNode()->yaw(Degree(-90));
        mCamera->getParentSceneNode()->pitch(Degree(-30));

        //Cria o terreno
        //mSceneMgr->setWorldGeometry("terrain.cfg");
    }


    void createFrameListener()
    {
        //adiciona a FrameListener criada acima para executar
        mFrameListener = new RobotWalkListener(mWindow, mCamera, mSceneMgr, mEntity);
        mFrameListener->showDebugOverlay(false);
        mRoot->addFrameListener(mFrameListener);
    }
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    SampleApp app;

    try
    {
        app.go();
    }
    catch( Exception& e )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else

        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }

    return 0;
}
