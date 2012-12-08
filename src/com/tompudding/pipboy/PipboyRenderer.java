/*
 * Copyright (C) 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tompudding.pipboy;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.os.SystemClock;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.opengles.GL10;

public class PipboyRenderer implements GLView.Renderer{

    public PipboyRenderer(Context context) {
        mContext = context;
        //mGlare = new Triangle(1.0f,1.0f,1.0f,1.0f);
        //mBand = new Triangle(1.0f,0.27f,4.0f,1.0f);
    }

    public int[] getConfigSpec() {
        // We don't need a depth buffer, and don't care about our
        // color depth.
        int[] configSpec = {
                EGL10.EGL_DEPTH_SIZE, 0,
                EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_NONE
        };
        return configSpec;
    }

    public void surfaceCreated(GL10 gl) {
        NativePipboy.init();
    }

    public void drawFrame(GL10 gl) {
        NativePipboy.step();
    }

    public void sizeChanged(GL10 gl, int w, int h) {
        gl.glViewport(0, 0, w, h);

        /*
        * Set our projection matrix. This doesn't have to be done
        * each time we draw, but usually a new projection needs to
        * be set when the viewport is resized.
        */

        float ratio = (float) w / h;
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadIdentity();
        if(w != h)
            GLU.gluOrtho2D(gl, 1, 0, 1, 0);
        //gl.glOrthof (0, 1, 0, 1, -1, 1);

        
        Log.w("nativepipboy", String.format("%d %d\n", w, h));
        
        //gl.glFrustumf(-ratio, ratio, -1, 1, 3, 7);

    }

    private Context mContext;
    private Pipboy mGlare;
    private Pipboy mBand;
    private int NUM_TEXTURES=3;
    private int[] mTextureID = {0,0,0};
}

class Pipboy {
    public Pipboy(float width, float height, float texwidth, float texheight) {

        // Buffers to be passed to gl*Pointer() functions
        // must be direct, i.e., they must be placed on the
        // native heap where the garbage collector cannot
        // move them.
        //
        // Buffers with multi-byte datatypes (e.g., short, int, float)
        // must have their byte order set to native order

        ByteBuffer vbb = ByteBuffer.allocateDirect(VERTS * 3 * 4);
        vbb.order(ByteOrder.nativeOrder());
        mFVertexBuffer = vbb.asFloatBuffer();

        ByteBuffer tbb = ByteBuffer.allocateDirect(VERTS * 2 * 4);
        tbb.order(ByteOrder.nativeOrder());
        mTexBuffer = tbb.asFloatBuffer();

        ByteBuffer ibb = ByteBuffer.allocateDirect(VERTS * 2);
        ibb.order(ByteOrder.nativeOrder());
        mIndexBuffer = ibb.asShortBuffer();

        // A unit-sided equalateral triangle centered on the origin.
        float[] coords = {
                // X, Y, Z
                0, 0, 0,
                0, height, 0,
                width, height, 0,
                width, 0, 0
        };

        float[] texcoords = {
            0,0,
            0,texheight,
            texwidth,texheight,
            texwidth,0
        };

        for (int i = 0; i < VERTS; i++) {
            for(int j = 0; j < 3; j++) {
                mFVertexBuffer.put(coords[i*3+j]);
            }
        }

        for (int i = 0; i < VERTS; i++) {
            for(int j = 0; j < 2; j++) {
                mTexBuffer.put(texcoords[i*2+j]);
            }
        }

        for(int i = 0; i < VERTS; i++) {
            mIndexBuffer.put((short) i);
        }

        mFVertexBuffer.position(0);
        mTexBuffer.position(0);
        mIndexBuffer.position(0);
    }

    public void draw(GL10 gl) {
        gl.glFrontFace(GL10.GL_CCW);
        gl.glVertexPointer(3, GL10.GL_FLOAT, 0, mFVertexBuffer);
        gl.glEnable(GL10.GL_TEXTURE_2D);
        gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, mTexBuffer);
        gl.glDrawElements(GL10.GL_TRIANGLE_FAN, VERTS,
                GL10.GL_UNSIGNED_SHORT, mIndexBuffer);
    }

    private final static int VERTS = 4;

    private FloatBuffer mFVertexBuffer;
    private FloatBuffer mTexBuffer;
    private ShortBuffer mIndexBuffer;
}
