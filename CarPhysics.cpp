
#include "CarPhysics.h"
#include <cmath>
#include <vector>



CarPhysics::CarPhysics(void): m_engineForce(0.f), m_breakingForce(0.f), m_vehicleSteering(0.f), turned(false),
                              m_lastCheckpointPos(0.f, 2.f, 0.f)
{                             
}    

CarPhysics::~CarPhysics(void)
{
}

btCollisionShape* CarPhysics::CreateVehicleShape()
{
    static btScalar baseVertices[] = {                     
        -0.45f*CAR_SCALE, -0.204f*CAR_SCALE, -2.276f*CAR_SCALE,   // back center
        -0.45f*CAR_SCALE, -0.472f*CAR_SCALE, -2.238f*CAR_SCALE,   // back lower
        -0.85f*CAR_SCALE, -0.597f*CAR_SCALE, -1.789f*CAR_SCALE,   // back bottom
        -0.85f*CAR_SCALE, -0.615f*CAR_SCALE,  1.728f*CAR_SCALE,   // front new bottom
        -0.35f*CAR_SCALE, -0.620f*CAR_SCALE,  2.198f*CAR_SCALE,   // front bottom 
        -0.35f*CAR_SCALE, -0.276f*CAR_SCALE,  2.273f*CAR_SCALE,   // front center
        -0.35f*CAR_SCALE, -0.046f*CAR_SCALE,  2.122f*CAR_SCALE,   // front upper
        -0.85f*CAR_SCALE,  0.069f*CAR_SCALE,  1.728f*CAR_SCALE,   // front new top
        -0.85f*CAR_SCALE,  0.154f*CAR_SCALE,  1.118f*CAR_SCALE,   // front top
        -0.85f*CAR_SCALE,  0.264f*CAR_SCALE, -1.827f*CAR_SCALE,   // back top
        -0.45f*CAR_SCALE,  0.207f*CAR_SCALE, -2.174f*CAR_SCALE,   // back upper    

         0.45f*CAR_SCALE, -0.204f*CAR_SCALE, -2.276f*CAR_SCALE,   // back center
         0.45f*CAR_SCALE, -0.472f*CAR_SCALE, -2.238f*CAR_SCALE,   // back lower
         0.85f*CAR_SCALE, -0.597f*CAR_SCALE, -1.789f*CAR_SCALE,   // back bottom
         0.85f*CAR_SCALE, -0.615f*CAR_SCALE,  1.728f*CAR_SCALE,   // front new bottom
         0.35f*CAR_SCALE, -0.620f*CAR_SCALE,  2.198f*CAR_SCALE,   // front bottom 
         0.35f*CAR_SCALE, -0.276f*CAR_SCALE,  2.273f*CAR_SCALE,   // front center
         0.35f*CAR_SCALE, -0.046f*CAR_SCALE,  2.122f*CAR_SCALE,   // front upper
         0.85f*CAR_SCALE,  0.069f*CAR_SCALE,  1.728f*CAR_SCALE,   // front new top
         0.85f*CAR_SCALE,  0.154f*CAR_SCALE,  1.118f*CAR_SCALE,   // front top
         0.85f*CAR_SCALE,  0.264f*CAR_SCALE, -1.827f*CAR_SCALE,   // back top
         0.45f*CAR_SCALE,  0.207f*CAR_SCALE, -2.174f*CAR_SCALE,   // back upper    
    };

    static btScalar topVertices[] = {                     
        -0.85f*CAR_SCALE,  0.264f*CAR_SCALE, -1.827f*CAR_SCALE,  // back bottom
        -0.85f*CAR_SCALE,  0.154f*CAR_SCALE,  1.118f*CAR_SCALE,  // front bottom
        -0.57f*CAR_SCALE,  0.596f*CAR_SCALE,  0.178f*CAR_SCALE,  // front top
        -0.57f*CAR_SCALE,  0.577f*CAR_SCALE, -1.038f*CAR_SCALE,  // back top

         0.85f*CAR_SCALE,  0.264f*CAR_SCALE, -1.827f*CAR_SCALE,  // back bottom
         0.85f*CAR_SCALE,  0.154f*CAR_SCALE,  1.118f*CAR_SCALE,  // front bottom
         0.57f*CAR_SCALE,  0.596f*CAR_SCALE,  0.178f*CAR_SCALE,  // front top
         0.57f*CAR_SCALE,  0.577f*CAR_SCALE, -1.038f*CAR_SCALE,  // back top        
    };

    btConvexHullShape *baseHull = new btConvexHullShape(baseVertices, 22, 3 * sizeof(btScalar)); 
    btConvexHullShape *topHull  = new btConvexHullShape( topVertices,  8, 3 * sizeof(btScalar)); 

    btCompoundShape* compound = new btCompoundShape();
    //m_collisionShapes.push_back(compound);

    // localTrans effectively shifts the center of mass with respect to the chassis
    btTransform localTrans = PhysicsUtils::btTransFrom(btVector3(0, m_cfg.bodyConnectionToChasisHeight, 0));
    
    compound->addChildShape(localTrans, baseHull);
    compound->addChildShape(localTrans, topHull);

    return compound;
}

void CarPhysics::Initialize(btDiscreteDynamicsWorld *refWorld, const btTransform & trans)
{
    m_refDynamicsWorld = refWorld;
    
    m_carChassis = PhysicsUtils::CreateRigidBody(m_cfg.mass, trans, CreateVehicleShape());
    m_refDynamicsWorld->addRigidBody(m_carChassis);
    m_carChassis->setDamping(m_cfg.linearDamping, m_cfg.angularDamping);    

    /// create vehicle
    m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_refDynamicsWorld);
    
    btRaycastVehicle::btVehicleTuning  m_tuning;
    m_vehicle = new btRaycastVehicle(m_tuning, m_carChassis, m_vehicleRayCaster);
    m_refDynamicsWorld->addVehicle(m_vehicle);        
    
    m_carChassis->setActivationState(DISABLE_DEACTIVATION);  ///never deactivate the vehicle
    m_vehicle->setCoordinateSystem(0, 1, 2);  //choose coordinate system
            
    // add wheels
    btVector3 connectionPointCS0;

    // front left
    connectionPointCS0 = btVector3( m_cfg.connectionWidth, m_cfg.connectionHeight,  m_cfg.connectionLength);
    m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, m_tuning, true);

    // front right
    connectionPointCS0 = btVector3(-m_cfg.connectionWidth, m_cfg.connectionHeight,  m_cfg.connectionLength);
    m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, m_tuning, true);

    // back left
    connectionPointCS0 = btVector3( m_cfg.connectionWidth, m_cfg.connectionHeight, -m_cfg.connectionLength);
    m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, m_tuning, false);

    // back right
    connectionPointCS0 = btVector3(-m_cfg.connectionWidth, m_cfg.connectionHeight, -m_cfg.connectionLength);
    m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, m_tuning, false);
    
    for (int i = 0; i < m_vehicle->getNumWheels(); i++)
    {
        btWheelInfo& wheel = m_vehicle->getWheelInfo(i);

        wheel.m_suspensionStiffness = m_cfg.suspensionStiffness;
        wheel.m_wheelsDampingRelaxation = m_cfg.suspensionDampingRelaxation;
        wheel.m_wheelsDampingCompression = m_cfg.suspensionDampingCompression;
        wheel.m_frictionSlip = m_cfg.wheelFrictionSlip;
        wheel.m_rollInfluence = m_cfg.rollInfluence;
        wheel.m_maxSuspensionTravelCm = m_cfg.suspensionMaxTravelCm;
    }
}

void CarPhysics::Deinitialize()
{




}

void CarPhysics::Reset()
{
    m_vehicleSteering = 0.f;
    
    btTransform tr = PhysicsUtils::btTransFrom(btVector3(37.19f, 9.5f, -21.7f), btQuaternion(btVector3(0, 1, 0), -3.14159f/2.f));
    //tr.setIdentity();
   // tr.setRotation(btQuaternion(btVector3(0.f, 1.f, 0.f), 3.14f));
    //tr.setOrigin(m_lastCheckpointPos);
    m_carChassis->setCenterOfMassTransform(tr);
    m_carChassis->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
    m_carChassis->setAngularVelocity(btVector3(0.f, 0.f, 0.f));
    

    m_refDynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(), m_refDynamicsWorld->getDispatcher());
    if (m_vehicle)
    {
        m_vehicle->resetSuspension();
        for (int i = 0; i < m_vehicle->getNumWheels(); i++)
            m_vehicle->updateWheelTransform(i, true); //synchronize the wheels with the (interpolated) chassis worldtransform
    }
}

void CarPhysics::Update(btScalar timeStep)
{
    btScalar speed = std::abs(m_vehicle->getCurrentSpeedKmHour()*10.f);
       
/*    if (speed > 100.f)
    {
        m_cfg.steeringClamp = 0.1f;
        m_cfg.steeringIncrement = 0.004f;
    }
    else */
 /*   {
        m_cfg.steeringClamp = 0.1f + 0.65f - ((0.65f * speed) / 100.f);
        m_cfg.steeringIncrement = 0.04f - ((0.004f * speed) / 100.f);
    }    */   
    
    
    m_vehicle->applyEngineForce(m_engineForce, WHEEL_REARLEFT);
    m_vehicle->setBrake(m_breakingForce, WHEEL_REARLEFT);
    m_vehicle->applyEngineForce(m_engineForce, WHEEL_REARRIGHT);
    m_vehicle->setBrake(m_breakingForce, WHEEL_REARLEFT);

    
    m_vehicle->setSteeringValue(m_vehicleSteering, WHEEL_FRONTLEFT);
    m_vehicle->setSteeringValue(m_vehicleSteering, WHEEL_FRONTRIGHT);

    m_engineForce = 0.f;
    m_breakingForce = 0.f;
    
    if (!turned)
    {
        if (m_vehicleSteering > 0.f)
        {
            m_vehicleSteering -= m_cfg.steeringIncrement;
            if (m_vehicleSteering <= 0.f)
                m_vehicleSteering = 0.f;
        }
        else
        {
            m_vehicleSteering += m_cfg.steeringIncrement;
            if (m_vehicleSteering >= 0.f)
                m_vehicleSteering = 0.f;
        }
    }
    turned = false;
}

void CarPhysics::TurnLeft()
{
    m_vehicleSteering += m_cfg.steeringIncrement;
    if (m_vehicleSteering > m_cfg.steeringClamp)
        m_vehicleSteering = m_cfg.steeringClamp;    
    turned = true;
}

void CarPhysics::TurnRight()
{
    m_vehicleSteering -= m_cfg.steeringIncrement;
    if (m_vehicleSteering < -m_cfg.steeringClamp)
        m_vehicleSteering = -m_cfg.steeringClamp;  
    turned = true;
}

void CarPhysics::Forward()
{
    if (std::abs(m_vehicle->getCurrentSpeedKmHour()) < 60)
        m_engineForce = m_cfg.maxEngineForce;
    m_breakingForce = 0.f;    
}

void CarPhysics::Backward()
{
    m_engineForce = -m_cfg.maxEngineForce;
    m_breakingForce = 0.f;    
}

void CarPhysics::HandBrake()
{   
    m_breakingForce = m_cfg.maxBreakingForce; 
    m_engineForce = 0.f;    
}

btTransform CarPhysics::GetWorldTransform()
{        
    btTransform vehicleTrans;
    m_vehicle->getRigidBody()->getMotionState()->getWorldTransform(vehicleTrans);
    btCompoundShape* compoundShape = static_cast<btCompoundShape*>(m_vehicle->getRigidBody()->getCollisionShape());
    return vehicleTrans * compoundShape->getChildTransform(0);
}
