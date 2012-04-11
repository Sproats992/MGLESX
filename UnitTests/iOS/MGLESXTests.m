//
//  MGLESXTests.m
//  MGLESXTests
//
//  Created by Robert Sproats on 30/03/2012.
//  Copyright (c) 2012 Mnemonic Studios Ltd. All rights reserved.
//

#import "MGLESXTests.h"
#include "MGLESX.h"

@implementation MGLESXTests

float current_tolerance_percentage = 0.01f;

- (void)setUp
{
    [super setUp];
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    [super tearDown];
}


-(Boolean)floatingPointTest:(float)value_one value_two:(float)value_two tolerance_percentage:(float)tolerance_percentage
{
    double tolerance_range = (value_one/100.0f)*tolerance_percentage;
    if (tolerance_range<0.0f) tolerance_range = -tolerance_range;
    if (tolerance_range<0.000001f) tolerance_range = 0.000001f; //fix the range if zero or too small.
    
    double difference = value_one - value_two;
    if (difference<0.0f) difference = -difference;
    
    if (difference>tolerance_range) {
        NSLog(@"Failed floating point comparison: %f, %f, %f, %f, %f", value_one, value_two, difference, tolerance_range, tolerance_percentage);
        return false;
    } else {
        //we're ok.
    }
    
    return true;
}

-(Boolean)compare4x4Matrix:(float*)matrix_one matrix_two:(float*)matrix_two {
    
    int matrix_loop = 0;
    Boolean result_to_return = true;
    for (matrix_loop=0; matrix_loop<16; matrix_loop++) {
        if ([self floatingPointTest:matrix_one[matrix_loop] value_two:matrix_two[matrix_loop] tolerance_percentage:current_tolerance_percentage]==false) { NSLog(@"matrix elements %d not equal to: %f and %f", matrix_loop, matrix_one[matrix_loop], matrix_two[matrix_loop]);
            result_to_return = false;
        }
    }
    
    return result_to_return;
    
}

-(Boolean)compare3x3Matrix:(float*)matrix_one matrix_two:(float*)matrix_two {
    
    int matrix_loop = 0;
    Boolean result_to_return = true;
    for (matrix_loop=0; matrix_loop<9; matrix_loop++) {
        if ([self floatingPointTest:matrix_one[matrix_loop] value_two:matrix_two[matrix_loop] tolerance_percentage:current_tolerance_percentage]==false) { NSLog(@"matrix elements %d not equal to: %f and %f", matrix_loop, matrix_one[matrix_loop], matrix_two[matrix_loop]);
            result_to_return = false;
        }
    }
    
    return result_to_return;
    
}

-(void)log4x4Matrix:(float*)matrix_to_log
{
    
    NSLog(@"Logging 4x4 matrix:");
    NSLog(@"%f, %f, %f, %f", matrix_to_log[0], matrix_to_log[1], matrix_to_log[2], matrix_to_log[3]);   
    NSLog(@"%f, %f, %f, %f", matrix_to_log[4], matrix_to_log[5], matrix_to_log[6], matrix_to_log[7]);   
    NSLog(@"%f, %f, %f, %f", matrix_to_log[8], matrix_to_log[9], matrix_to_log[10], matrix_to_log[11]);   
    NSLog(@"%f, %f, %f, %f", matrix_to_log[12], matrix_to_log[13], matrix_to_log[14], matrix_to_log[15]);   
    
    
}

-(void)verifyIdentityMatrix:(float*)matrix_to_verify
{
    MGLESXMatrix4x4 identity_matrix = { 
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    
    //verifying identity matrix
    [self compare4x4Matrix:matrix_to_verify matrix_two:&identity_matrix[0]];
    
}

//Test to verify loading identity matrix.
- (void)testIdentityMatrixFunctions
{
    
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    
    MGLESX_loadIdentity();
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
}

-(void)testCreatePerspective
{

    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    
    MGLESX_loadIdentity();
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_createPerspective(65.0f, 0.75f, 0.1f, 100.0f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        2.092914, 0.000000, 0.000000, 0.000000,
        0.000000, 1.569686, 0.000000, 0.000000,
        0.000000, 0.000000, -1.002002, -1.000000,
        0.000000, 0.000000, -0.200200, 0.000000
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Create perspective matrix 1 does not match test data.");


}

-(void)testLookAt
{
    //void MGLESX_lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
    
    
}
-(void)testMatrixOperations
{
    
    //test one - check correct 3x3 matrix is cropped from 4x4 matrix.
    MGLESXMatrix4x4 test_data_matrix = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    
    MGLESXMatrix3x3 test_results_matrix = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    
    MGLESX_get3x3MatrixFrom4x4Matrix(&test_data_matrix[0] , &test_results_matrix[0]);
    
    MGLESXMatrix4x4 test_comparison_matrix = { 
        1.0f, 2.0f, 3.0f, 
        5.0f, 6.0f, 7.0f, 
        9.0f, 10.0f, 11.0f
    };
    
    STAssertTrue([self compare3x3Matrix:&test_results_matrix[0] matrix_two:&test_comparison_matrix[0]], @"Matrix operations 1 does not match test data.");

    
    //test two - check correct 3x3 matrix is inverted and transposed.
    MGLESXMatrix3x3 test2_data_matrix = {
        -0.215996, 0.957403, -0.191636,
        0.974238, 0.224364, 0.022832,
        0.064856, -0.181768, -0.981200
    };
    
    MGLESXMatrix3x3 test2_results_matrix = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };
    
    MGLESX_matrix3x3InvertAndTranspose(&test2_data_matrix[0] , &test2_results_matrix[0]);
    
    MGLESXMatrix4x4 test2_comparison_matrix = { 
        -0.215996, 0.957403, -0.191636,
        0.974238, 0.224364, 0.022832,
        0.064856, -0.181768, -0.981201
    };
    
    STAssertTrue([self compare3x3Matrix:&test2_results_matrix[0] matrix_two:&test2_comparison_matrix[0]], @"Matrix operations 2 does not match test data.");
    
}

//Test to verify frustum projections are loaded into the matrix.
- (void)testLoadFrustumFunctions
{
    
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    
    MGLESX_loadIdentity();
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_frustumf(-0.352082, 0.352082, -0.414214, 0.414214, 1.000000, 300.000000);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        2.840251f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 2.414213f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, -1.006689f, -1.000000f,
        -0.000000f, -0.000000f, -2.006689f, -0.000000f,
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Frustum matrix 1 does not match test data.");
    
    //test two
    MGLESX_loadIdentity();
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_frustumf(-85.0f, 85.0f, -100.0f , 100.0f, 10.0f, 15.0f);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        0.117647f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.100000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, -5.000000f, -1.000000f,
        -0.000000f, -0.000000f, -60.000000f, -0.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Frustum matrix 2 does not match test data.");
    
    //test three - crazy values for near and far.
    MGLESX_loadIdentity();
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_frustumf(1.25f, 4.75f, 32.0f , 0.04f, -10.0f, 0.01f);
    
    MGLESXMatrix4x4 test3_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f,
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test3_results_matrix[0]], @"Frustum matrix 3 does not match test data.");
    
    //test four - irregular frustum.
    MGLESX_loadIdentity();
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_frustumf(1.25f, 4.75f, 32.0f , 0.04f, 1.0f, 2.0f);
    
    MGLESXMatrix4x4 test4_results_matrix = { 
        0.571429f, 0.000000f, 0.000000f, 0.000000f,
        -0.000000f, -0.062578f, -0.000000f, -0.000000f,
        1.714286f, -1.002503f, -3.000000f, -1.000000f,
        -0.000000f, -0.000000f, -4.000000f, -0.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4_results_matrix[0]], @"Frustum matrix 4 does not match test data.");
    
    //test five - irregular frustum with double setting.
    MGLESX_loadIdentity();
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_frustumf(-85.0f, 85.0f, -100.0f , 100.0f, 10.0f, 15.0f);
    MGLESX_frustumf(1.25f, -4.75f, -32.0f , 0.04f, 1.0f, 2.0f);
    
    MGLESXMatrix4x4 test5_results_matrix = { 
        -0.333333f, -0.000000f, -0.000000f, -0.000000f,
        0.000000f, 0.062422f, 0.000000f, 0.000000f,
        0.583333f, -0.997503f, -3.000000f, -1.000000f,
        -0.000000f, -0.000000f, -4.000000f, -0.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test5_results_matrix[0]], @"Frustum matrix 5 does not match test data.");
    
    
    
}

//Test to verify matrix can be loaded with unique values.
- (void)testLoadMatrixFunctions
{
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    
    //test one
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESXMatrix4x4 false_matrix_results = { 2.0f, 3.0f, 4.0f, 5.0f,
        6.0f, 7.0f, 8.0f, 9.0f,
        10.0f, 11.0f, 12.0f, 13.0f,
        14.0f, 15.0f, 16.0f, 17.0f};
    
    MGLESX_loadMatrixf(&false_matrix_results[0]);
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&false_matrix_results[0]], @"Loaded matrix does not match test data.");
    
}

//Test to verify loading of orthographic projections into the matrix.
- (void)testLoadOrthoFunctions
{
    
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    //test one.
    MGLESX_orthof(0.0, 320.0, 0.0, 480.0, -1.0f, 1.0f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        0.006250f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.004167f, 0.000000f, 0.000000f,
        -0.000000f, -0.000000f, -1.000000f, -0.000000f,
        -1.000000f, -1.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Ortho matrix 1 does not match test data.");
    
    
    //test two.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_orthof(-160.f, 160.0f, -240.0f, 240.0, 1.0, 300.0);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        0.006250f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.004167f, 0.000000f, 0.000000f,
        -0.000000f, -0.000000f, -0.006689f, -0.000000f,
        0.000000f, 0.000000f, -1.006689f, 1.000000f,
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Ortho matrix 2 does not match test data.");
    
    
    //test three.
    MGLESX_loadIdentity();
    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_orthof(160.0, -160.0, 240.0, -480.0, -1.0, 250.0);
    
    MGLESXMatrix4x4 test3_results_matrix = { 
        -0.006250f, -0.000000f, -0.000000f, -0.000000f,
        -0.000000f, -0.002778f, -0.000000f, -0.000000f,
        -0.000000f, -0.000000f, -0.007968f, -0.000000f,
        0.000000f, -0.333333f, -0.992032f, 1.000000f,
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test3_results_matrix[0]], @"Ortho matrix 3 does not match test data.");
    
    //test four - two in a row. Would never use this, but proves it works in the same way.
    MGLESX_loadIdentity();
    
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_orthof(0.0f, 320.0f, 0.0f, 480.0f, 1.0f, -1.0f);
    MGLESX_orthof(160.0, -160.0, 240.0, -480.0, -1.0, 250.0);
    
    MGLESXMatrix4x4 test4_results_matrix = { 
        -0.006250f, -0.000000f, -0.000000f, -0.000000f,
        -0.000000f, -0.002778f, -0.000000f, -0.000000f,
        -0.000000f, -0.000000f, -0.007968f, -0.000000f,
        0.000000f, -0.333333f, -0.992032f, 1.000000f,
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4_results_matrix[0]], @"Ortho matrix 4 does not match test data.");
}

//Test to exercise the matrix stacks for modelview and projection and verify correct errors are produced.
- (void)testMatrixStackFunctionsAndErrors {
    
    //test one. Check model view stack blows at correct point.
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_MODEL_VIEW);
    MGLESX_loadIdentity();
    
    int matrix_stack_depth = 0;
    int current_error_code = 0;
    
    //wind up the stack and check it blows at the correct point.
    while (current_error_code==0) {
        MGLESX_pushMatrix();
        matrix_stack_depth++;
        current_error_code = MGLESX_getError();
    }
    
    //check depth is correct and the correct error is returned.
    STAssertTrue(matrix_stack_depth==32, @"Max matrix stack depth is not 32.");
    STAssertTrue(current_error_code==MGLESX_ERROR_CODE_OVERFLOW_MODELVIEW_MATRIX_STACK, @"Error code was not MGLESX_ERROR_CODE_OVERFLOW_MODELVIEW_MATRIX_STACK.");
    
    current_error_code = 0;
    
    //wind down the stack and check it blows at the correct point.
    while (current_error_code==0) {
        MGLESX_popMatrix();
        matrix_stack_depth--;
        current_error_code = MGLESX_getError();
    }    
    
    //check depth is correct and the correct error is returned.
    STAssertTrue(matrix_stack_depth==0, @"Min matrix stack depth is not 0.");
    STAssertTrue(current_error_code==MGLESX_ERROR_CODE_UNDERFLOW_MODELVIEW_MATRIX_STACK, @"Error code was not MGLESX_ERROR_CODE_UNDERFLOW_MODELVIEW_MATRIX_STACK.");
    
    //test two. Check projection stack blows at correct point.
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_PROJECTION);
    MGLESX_loadIdentity();
    
    matrix_stack_depth = 0;
    current_error_code = 0;
    
    //wind up the stack and check it blows at the correct point.
    while (current_error_code==0) {
        MGLESX_pushMatrix();
        matrix_stack_depth++;
        current_error_code = MGLESX_getError();
    }
    
    //check depth is correct and the correct error is returned.
    STAssertTrue(matrix_stack_depth==4, @"Max matrix stack depth is not 4.");
    STAssertTrue(current_error_code==MGLESX_ERROR_CODE_OVERFLOW_PROJECTION_MATRIX_STACK, @"Error code was not MGLESX_ERROR_CODE_OVERFLOW_PROJECTION_MATRIX_STACK.");
    
    current_error_code = 0;
    
    //wind down the stack and check it blows at the correct point.
    while (current_error_code==0) {
        MGLESX_popMatrix();
        matrix_stack_depth--;
        current_error_code = MGLESX_getError();
    }    
    
    //check depth is correct and the correct error is returned.
    STAssertTrue(matrix_stack_depth==0, @"Min matrix stack depth is not 0.");
    STAssertTrue(current_error_code==MGLESX_ERROR_CODE_UNDERFLOW_PROJECTION_MATRIX_STACK, @"Error code was not MGLESX_ERROR_CODE_UNDERFLOW_PROJECTION_MATRIX_STACK.");
    
}

// Test to exercise multiple transformations on a single matrix.
- (void)testCombinedFunctions {
    
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_MODEL_VIEW);
    
    //test one x rotation, y translation, z scaling.
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(24.35f, 1.0f, 0.0f, 0.0f);
    MGLESX_translatef(0.0f, 32.55f, 0.0f);
    MGLESX_scalef(1.0f, 1.0f, 1.27f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.911044f, 0.412310f, 0.000000f,
        0.000000f, -0.523633f, 1.157026f, 0.000000f,
        0.000000f, 29.654476f, 13.420676f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Combined matrix 1 does not match test data.");
    
    //test two everything!
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(-212.07f, 1.0f, 0.0f, 0.0f);
    MGLESX_translatef(-55.2f, 32.55f, -1.11752f);
    MGLESX_scalef(20.24f, 1.75f, 1.27f);
    MGLESX_rotatef(45.95f, 0.0f, 1.0f, 0.0f);
    MGLESX_rotatef(11.0f, 0.0f, 0.0f, 1.0f);
    MGLESX_scalef(0.25f, 0.75f, -0.55f);
    MGLESX_translatef(982.6342f, -7532.643f, 64.2434f);
    MGLESX_rotatef(48683.47753f, 0.1f, 0.1f, 0.8f);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        -1.439737f, -1.149304f, 0.592306f, 0.000000f,
        -5.426329f, -0.134784f, -0.058541f, 0.000000f,
        -6.962727f, 0.279256f, 0.445766f, 0.000000f,
        17994.179688f, 8783.351562f, -4044.333984f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Combined matrix 1 does not match test data.");
    
}

//Test to verify rotation transformations and operating with the matrix stack.
- (void)testModelviewRotationFunctions
{
    
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_MODEL_VIEW);
    
    //test one x rotation.
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(24.35f, 1.0f, 0.0f, 0.0f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.911044f, 0.412310f, 0.000000f,
        0.000000f, -0.412310f, 0.911044f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f    
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Rotation matrix 1 does not match test data.");
    
    //test two y rotation.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(-719.21f, 0.0f, -1.0f, 0.0f);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        0.999905f, 0.000000f, 0.013787f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        -0.013787f, 0.000000f, 0.999905f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f    
    };
    
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Rotation matrix 2 does not match test data.");
    
    //test three z rotation.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(89.67f, 0.0f, 0.0f, 1.0f);
    
    MGLESXMatrix4x4 test3_results_matrix = { 
        0.005760f, 0.999983f, 0.000000f, 0.000000f,
        -0.999983f, 0.005760f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test3_results_matrix[0]], @"Rotation matrix 3 does not match test data.");
    
    //test four all rotations together.
    MGLESX_loadIdentity();
    
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_rotatef(-49.81f, 3.21f, -67.29f, -99.0f);
    
    MGLESXMatrix4x4 test4_results_matrix = { 
        0.645579f, 0.626216f, -0.437129f, 0.000000f,
        -0.636901f, 0.757321f, 0.144297f, 0.000000f,
        0.421408f, 0.185253f, 0.887748f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4_results_matrix[0]], @"Rotation matrix 4 does not match test data.");
    
    //test five combined rotations and stack operations.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_pushMatrix(); {
        
        MGLESX_rotatef(-49.81f, 1.0f, 0.0f, 0.0f);
        
        MGLESX_pushMatrix(); {
            
            MGLESX_rotatef(0.0125f, 0.0f, 1.0f, 0.0f);
            
            MGLESX_pushMatrix(); {
                
                MGLESX_rotatef(4096.0f, 0.0f, 0.0f, 1.0f);
                
                MGLESXMatrix4x4 test5c_results_matrix = { 
                    -0.719339f, 0.448400f, -0.530555f, 0.000000f,
                    -0.694659f, -0.464091f, 0.549607f, -0.000000f,
                    0.000218f, 0.763909f, 0.645324f, 0.000000f,
                    0.000000f, 0.000000f, 0.000000f, 1.000000f
                };
                
                test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
                
                STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test5c_results_matrix[0]], @"Rotation matrix 5c does not match test data.");
                
            } MGLESX_popMatrix();
            
            MGLESXMatrix4x4 test5b_results_matrix = { 
                1.000000f, -0.000167f, -0.000141f, 0.000000f,
                0.000000f, 0.645324f, -0.763909f, 0.000000f,
                0.000218f, 0.763909f, 0.645324f, 0.000000f,
                0.000000f, 0.000000f, 0.000000f, 1.000000f
            };
            
            test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
            
            STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test5b_results_matrix[0]], @"Rotation matrix 5b does not match test data.");
            
            
        } MGLESX_popMatrix();
        
        MGLESXMatrix4x4 test5a_results_matrix = { 
            1.000000f, 0.000000f, 0.000000f, 0.000000f,
            0.000000f, 0.645324f, -0.763909f, 0.000000f,
            0.000000f, 0.763909f, 0.645324f, 0.000000f,
            0.000000f, 0.000000f, 0.000000f, 1.000000f  
        };
        
        test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
        
        STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test5a_results_matrix[0]], @"Rotation matrix 5a does not match test data.");
        
    } MGLESX_popMatrix();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
}


//Test to verify translation transformations and operation with the matrix stack.
- (void)testModelviewTranslationFunctions
{
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_MODEL_VIEW);
    
    //test one x translation.
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_translatef(23.45f, 0.0f, 0.0f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        23.450000f, 0.000000f, 0.000000f, 1.000000f    
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Translation matrix 1 does not match test data.");
    
    //test two y translation.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_translatef(0.0f, 42.76f, 0.0f);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 42.760000f, 0.000000f, 1.000000f    
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Translation matrix 2 does not match test data.");
    
    //test three z translation.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_translatef(0.0f, 0.0f, -19.21f);
    
    MGLESXMatrix4x4 test3_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 0.000000f, -19.210000f, 1.000000f    
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test3_results_matrix[0]], @"Translation matrix 3 does not match test data.");
    
    //test four composite with stack translation.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_pushMatrix(); {
        
        MGLESX_translatef(0.0f, 0.0f, -19.21f);
        
        MGLESX_pushMatrix(); {
            
            MGLESX_translatef(0.0f, 42.76f, 0.0f);
            
            MGLESX_pushMatrix(); {
                
                MGLESX_translatef(23.45f, 0.0f, 0.0f);
                
                MGLESXMatrix4x4 test4c_results_matrix = { 
                    1.000000f, 0.000000f, 0.000000f, 0.000000f,
                    0.000000f, 1.000000f, 0.000000f, 0.000000f,
                    0.000000f, 0.000000f, 1.000000f, 0.000000f,
                    23.450000f, 42.760000f, -19.210000f, 1.000000f    
                };
                
                test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
                
                STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4c_results_matrix[0]], @"Translation matrix 4c does not match test data.");
                
                
            } MGLESX_popMatrix();
            
            MGLESXMatrix4x4 test4b_results_matrix = { 
                1.000000f, 0.000000f, 0.000000f, 0.000000f,
                0.000000f, 1.000000f, 0.000000f, 0.000000f,
                0.000000f, 0.000000f, 1.000000f, 0.000000f,
                0.000000f, 42.760000f, -19.210000f, 1.000000f    
            };
            
            test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
            
            STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4b_results_matrix[0]], @"Translation matrix 4b does not match test data.");
            
        } MGLESX_popMatrix();
        
        MGLESXMatrix4x4 test4a_results_matrix = { 
            1.000000f, 0.000000f, 0.000000f, 0.000000f,
            0.000000f, 1.000000f, 0.000000f, 0.000000f,
            0.000000f, 0.000000f, 1.000000f, 0.000000f,
            0.000000f, 0.000000f, -19.210000f, 1.000000f    
        };
        
        test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
        
        STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4a_results_matrix[0]], @"Translation matrix 4a does not match test data.");
        
    } MGLESX_popMatrix();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
}

//Test to verify scaling transformations and operation with the matrix stack.
- (void)testModelviewScalingFunctions
{
    MGLESX_set_matrix_mode(MGLESX_MATRIX_MODE_MODEL_VIEW);
    
    //test one x scaling.
    MGLESX_loadIdentity();
    
    MGLESXMatrix4x4* test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_scalef(0.5f, 1.0f, 1.0f);
    
    MGLESXMatrix4x4 test_results_matrix = { 
        0.500000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test_results_matrix[0]], @"Scaling matrix 1 does not match test data.");
    
    //test two y scaling.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_scalef(1.0f, 0.25f, 1.0f);
    
    MGLESXMatrix4x4 test2_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.250000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.000000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test2_results_matrix[0]], @"Scaling matrix 2 does not match test data.");
    
    //test three z scaling.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_scalef(1.0f, 1.0f, 0.75f);
    
    MGLESXMatrix4x4 test3_results_matrix = { 
        1.000000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.000000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 0.750000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test3_results_matrix[0]], @"Scaling matrix 3 does not match test data.");
    
    //test four composite scaling.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_scalef(0.125f, 4.73f, -2.95);
    
    MGLESXMatrix4x4 test4_results_matrix = { 
        0.125000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 4.730000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, -2.950000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test4_results_matrix[0]], @"Scaling matrix 4 does not match test data.");
    
    //test five composite series scaling.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_scalef(0.125f, 4.73f, -2.95);
    MGLESX_scalef(2.0f, 0.5f, -0.5f);
    
    MGLESXMatrix4x4 test5_results_matrix = { 
        0.250000f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 2.365000f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, 1.475000f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f
    };
    
    STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test5_results_matrix[0]], @"Scaling matrix 5 does not match test data.");
    
    //test six composite series with push and pop scaling.
    MGLESX_loadIdentity();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
    MGLESX_pushMatrix(); {
        
        test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
        MGLESX_scalef(0.125f, 4.73f, -2.95);
        
        MGLESX_pushMatrix(); {
            
            test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
            MGLESX_scalef(2.0f, 0.5f, -0.5f);
            
            MGLESXMatrix4x4 test6a_results_matrix = { 
                0.250000f, 0.000000f, 0.000000f, 0.000000f,
                0.000000f, 2.365000f, 0.000000f, 0.000000f,
                0.000000f, 0.000000f, 1.475000f, 0.000000f,
                0.000000f, 0.000000f, 0.000000f, 1.000000f
            };
            
            STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test6a_results_matrix[0]], @"Scaling matrix 6a does not match test data.");
            
        } MGLESX_popMatrix();
        
        test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
        
        MGLESXMatrix4x4 test6b_results_matrix = { 
            0.125000f, 0.000000f, 0.000000f, 0.000000f,
            0.000000f, 4.730000f, 0.000000f, 0.000000f,
            0.000000f, 0.000000f, -2.950000f, 0.000000f,
            0.000000f, 0.000000f, 0.000000f, 1.000000f
        };
        
        STAssertTrue([self compare4x4Matrix:test_matrix_pointer[0] matrix_two:&test6b_results_matrix[0]], @"Scaling matrix 6b does not match test data.");
        
    } MGLESX_popMatrix();
    
    test_matrix_pointer = MGLESX_getCurrentMatrixPointer();
    [self verifyIdentityMatrix:test_matrix_pointer[0]];
    
}


@end
