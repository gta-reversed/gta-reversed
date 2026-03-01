#pragma once

static constexpr auto NUM_HANDSHAKERS = 6;
class CHandShaker {
public:
  CVector m_ang;
  CVector m_lim;              //! Angle (`m_ang`) limit [That is, `m_ang` is clamped to [-m_lim, m_lim])
  CVector m_motion;           
  CVector m_vel;              ///! Motion velocity
  CVector m_slow;             ///! Slow motion velocity
  CMatrix m_resultMat;        ///!
  float   m_scaleReactionMin;
  float   m_scaleReactionMax;
  int32   m_twitchFreq;
  float   m_twitchVel;

public:
    static void InjectHooks();

    CHandShaker() { SetDefaults(); } // 0x517740
    ~CHandShaker() = default;        // 0x509810

    void SetDefaults();
    void Reset();
    void Process(float degree);
};

VALIDATE_SIZE(CHandShaker, 0x94);

static inline std::array<CHandShaker, NUM_HANDSHAKERS>& gHandShaker = StaticRef<std::array<CHandShaker, NUM_HANDSHAKERS>>(0xB6ECA0);
