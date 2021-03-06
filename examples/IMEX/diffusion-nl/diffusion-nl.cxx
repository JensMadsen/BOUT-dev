/*
 * Non-linear diffusion
 *
 * This test case solves the diffusion equation along y:
 *
 * df/dt = Div_par( D Grad_par( f ) )
 *
 * using a nonlinear coefficient D, which depends on f
 * to a specified power alpha:
 *
 * D = f ^ alpha
 *
 * This is a model for parallel heat conduction in plasmas,
 * where the heat conduction coefficient depends on T^{5/2}
 *
 */

#include <bout/physicsmodel.hxx>
#include <bout/fv_ops.hxx>
#include <invert_parderiv.hxx>

class DiffusionNL : public PhysicsModel {
protected:
  int init(bool restarting) {
    // Get the input parameter alpha
    Options *opt = Options::getRoot();
    OPTION(opt, alpha, 2.5);
    
    // Specify that the operator is split
    // into convective and diffusive parts
    setSplitOperator();

    // Specify the preconditioner function
    setPrecon( (preconfunc) &DiffusionNL::precon );

    // Add the field "f" to the time integration solver
    SOLVE_FOR(f);

    return 0;
  }
  /*!
   * Convective part of the problem. In an IMEX scheme
   * this will be treated explicitly
   */
  int convective(BoutReal time) {
    ddt(f) = 0.0;
    return 0;
  }
  
  /*!
   * Diffusive part of the problem. In an IMEX scheme
   * this will be treated implicitly
   * 
   * Inputs
   * ------
   * 
   * time    = Current simulation time
   * linear  = True if solver is in linear solve (PETSc KSP)
   * f       = Evolving variable (stored in this class)
   * 
   * Outputs
   * -------
   * ddt(f)  = Time derivative of f
   */
  int diffusive(BoutReal time, bool linear) {
    mesh->communicate(f);
    if(!linear) {
      // Update diffusion coefficient
      D = f ^ alpha;
    }

    // Finite volume parallel diffusion term
    ddt(f) = FV::Div_par_K_Grad_par(D, f);

    return 0;
  }

  /*!
   * Preconditioner. This inverts the operator (1 - gamma*J) 
   * where J is the Jacobian of the system
   *
   * The system state at time t is stored as usual (here in f)
   * whilst the vector to be inverted is in ddt(f)
   * 
   * Inputs
   * ------
   *
   * t      = Current simulation time
   * gamma  = Coefficient proportional to timestep
   * delta  = Coefficient used in contrained problems
   * f      = System state at current time
   * ddt(f) = Variable to be inverted
   *
   * Output
   * ------
   * 
   * ddt(f) = Result of the inversion
   */
  int precon(BoutReal t, BoutReal gamma, BoutReal delta) {
    // Preconditioner
    
    static InvertPar *inv = NULL;
    if(!inv) {
      // Initialise parallel inversion class
      inv = InvertPar::Create();
      inv->setCoefA(1.0);
    }

    // Set the coefficient in front of Grad2_par2
    inv->setCoefB(-gamma*D);
    //mesh->communicate(ddt(f));
    
    ddt(f) = inv->solve(ddt(f));
    
    return 0;
  }
private:
  Field3D f;      // Evolving quantity
  BoutReal alpha; // Input parameter, D = f^alpha
  Field3D D;      // The diffusion coefficient
};

BOUTMAIN(DiffusionNL);
