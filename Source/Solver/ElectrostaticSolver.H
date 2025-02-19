#include <AMReX.H>
#include <AMReX_MultiFab.H>
#include <AMReX_MultiFabUtil.H>
#include "FerroX.H"
#include "Input/BoundaryConditions/BoundaryConditions.H"
#include "Input/GeometryProperties/GeometryProperties.H"
#include <AMReX_MLABecLaplacian.H>
#include <AMReX_MLMG.H> 

#ifdef AMREX_USE_EB
#include <AMReX_MLEBABecLap.H>
#endif


using namespace amrex;
using namespace FerroX;

void ComputePoissonRHS(MultiFab&               PoissonRHS, 
		Array<MultiFab, AMREX_SPACEDIM> &P_old,
		MultiFab&                      rho, 
		MultiFab&                      MaterialMask, 
                MultiFab& angle_alpha, MultiFab& angle_beta, MultiFab& angle_theta,
		const Geometry&                 geom);

//void ComputeEfromPhi(MultiFab&                 PoissonPhi,
//                MultiFab&                      Ex,
//                MultiFab&                      Ey,
//                MultiFab&                      Ez,
//                const Geometry&                 geom);
//
void ComputeEfromPhi(MultiFab&                 PoissonPhi,
		Array<MultiFab, AMREX_SPACEDIM> &E,
                MultiFab& angle_alpha, MultiFab& angle_beta, MultiFab& angle_theta,
                const Geometry&                 geom,
                const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_lo,
                const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_hi);

//mask based permittivity
void InitializePermittivity(std::array<std::array<amrex::LinOpBCType,AMREX_SPACEDIM>,2>& LinOpBCType_2d, 
                MultiFab& beta_cc, 
                const MultiFab& MaterialMask, 
                const MultiFab& tphaseMask, 
                const amrex::GpuArray<int, AMREX_SPACEDIM>& n_cell, 
                const Geometry& geom, 
                const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_lo, 
                const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_hi);

void dF_dPhi(MultiFab&            alpha_cc,
             MultiFab&            PoissonRHS, 
             MultiFab&            PoissonPhi, 
	     Array<MultiFab, AMREX_SPACEDIM>& P_old,
             MultiFab&            rho,
             MultiFab&            e_den,
             MultiFab&            p_den,
	     MultiFab&      MaterialMask,
             MultiFab& angle_alpha, MultiFab& angle_beta, MultiFab& angle_theta,
             const          Geometry& geom,
	     const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_lo,
             const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_hi);

void ComputePoissonRHS_Newton(MultiFab& PoissonRHS, 
                              MultiFab& PoissonPhi, 
                              MultiFab& alpha_cc);

void SetPhiBC_z(MultiFab& PossonPhi, const amrex::GpuArray<int, AMREX_SPACEDIM>& n_cell, const Geometry& geom);

void SetPoissonBC(c_FerroX& rFerroX, std::array<std::array<amrex::LinOpBCType,AMREX_SPACEDIM>,2>& LinOpBCType_2d, bool& all_homogeneous_boundaries, bool& some_functionbased_inhomogeneous_boundaries, bool& some_constant_inhomogeneous_boundaries);

void Fill_Constant_Inhomogeneous_Boundaries(c_FerroX& rFerroX, MultiFab& PoissonPhi);
void Fill_FunctionBased_Inhomogeneous_Boundaries(c_FerroX& rFerroX, MultiFab& PoissonPhi, amrex::Real& time);

void CheckSteadyState(MultiFab& PoissonPhi, MultiFab& PoissonPhi_Old, MultiFab& Phidiff, Real phi_tolerance, int step, int& steady_state_step, int& inc_step);
void SetupMLMG(std::unique_ptr<amrex::MLMG>& pMLMG, 
        std::unique_ptr<amrex::MLABecLaplacian>& p_mlabec,
        std::array<std::array<amrex::LinOpBCType,AMREX_SPACEDIM>,2>& LinOpBCType_2d,
        const amrex::GpuArray<int, AMREX_SPACEDIM>& n_cell,
        std::array< MultiFab, AMREX_SPACEDIM >& beta_face,
        c_FerroX& rFerroX, MultiFab& PoissonPhi, amrex::Real& time, amrex::LPInfo& info);

#ifdef AMREX_USE_EB
void SetupMLMG_EB(std::unique_ptr<amrex::MLMG>& pMLMG, 
        std::unique_ptr<amrex::MLEBABecLap>& p_mlebabec,
        std::array<std::array<amrex::LinOpBCType,AMREX_SPACEDIM>,2>& LinOpBCType_2d,
        const amrex::GpuArray<int, AMREX_SPACEDIM>& n_cell,
        std::array< MultiFab, AMREX_SPACEDIM >& beta_face,
        MultiFab& beta_cc,
        c_FerroX& rFerroX, MultiFab& PoissonPhi, amrex::Real& time, amrex::LPInfo& info);
#endif

void ComputePhi_Rho(std::unique_ptr<amrex::MLMG>& pMLMG, 
             std::unique_ptr<amrex::MLABecLaplacian>& p_mlabec,
             MultiFab&            alpha_cc,
             MultiFab&            PoissonRHS, 
             MultiFab&            PoissonPhi, 
             MultiFab&            PoissonPhi_Prev,
             MultiFab&            PhiErr,  
	         Array<MultiFab, AMREX_SPACEDIM>& P_old,
             MultiFab&            rho,
             MultiFab&            e_den,
             MultiFab&            p_den,
	         MultiFab&            MaterialMask,
             MultiFab& angle_alpha, MultiFab& angle_beta, MultiFab& angle_theta,
             const          Geometry& geom,
	         const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_lo,
             const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_hi);

#ifdef AMREX_USE_EB
void ComputePhi_Rho_EB(std::unique_ptr<amrex::MLMG>& pMLMG, 
             std::unique_ptr<amrex::MLEBABecLap>& p_mlebabec,
             MultiFab&            alpha_cc,
             MultiFab&            PoissonRHS, 
             MultiFab&            PoissonPhi, 
             MultiFab&            PoissonPhi_Prev,
             MultiFab&            PhiErr,  
	         Array<MultiFab, AMREX_SPACEDIM>& P_old,
             MultiFab&            rho,
             MultiFab&            e_den,
             MultiFab&            p_den,
	         MultiFab&            MaterialMask,
             MultiFab& angle_alpha, MultiFab& angle_beta, MultiFab& angle_theta,
             const          Geometry& geom,
	         const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_lo,
             const amrex::GpuArray<amrex::Real, AMREX_SPACEDIM>& prob_hi);
#endif
