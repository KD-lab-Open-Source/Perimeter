#ifndef PERIMETER_EMPTYRENDERDEVICE_H
#define PERIMETER_EMPTYRENDERDEVICE_H


class cEmptyRender : public cInterfaceRenderDevice
{
public:
    cEmptyRender() = default;
    ~cEmptyRender() override;

    eRenderDeviceSelection GetRenderSelection() const override {
        return DEVICE_HEADLESS;
    }

    bool ChangeSize(int xScr,int yScr,int mode) override {return false;};
    int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) override { return -1; }
    int SetClipRect(int xmin,int ymin,int xmax,int ymax) override { return -1; }
    void UseOrthographicProjection() override {};
    void SetDrawTransform(class cCamera *pDrawNode) override {}
    void SetWorldMat4f(const Mat4f* matrix) override {};

    int BeginScene() override { return -1; }
    int EndScene() override { return -1; }
    int Fill(int r,int g,int b,int a=255) override { return -1; }
    int Flush(bool wnd=false) override { return -1; }
    int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) override { return -1; }

    // вспомогательные функции, могут быть не реализованы
    void DrawLine(int x1,int y1,int x2,int y2,sColor4c color) override { }
    void DrawPixel(int x1,int y1,sColor4c color) override { }
    void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false) override { }
    void FlushPrimitive2D() override { }
    void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color) override { }
    void DrawPoint(const Vect3f &v1,sColor4c color) override { }
    void FlushPrimitive3D() override { }

    void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override { }
    void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,
                         Vect2f uv,//Координаты текстуры в точке x,y
                         Vect2f duv,//du,dv на один логический пиксель 
            //(лог пиксель равен графическому в разрешении 1024x768)
                         float phase=0,
                         float lerp_factor=1//0..1 Насколько сильно влияет pTexture
    ) override { }


    bool SetScreenShot(const char *fname) override { return false; }
    uint32_t GetRenderState(eRenderStateOption option) { return 0; }
    int SetRenderState(eRenderStateOption option,uint32_t value) override { return 0; }
    void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,const sColor4c& Color=sColor4c(255,255,255,255)) override { }

    int Create(class cTileMap *TileMap) override { return -1; }
    int Delete(class cTileMap *TileMap) override { return -1; }

    void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                            cTexture *Texture,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) override { }
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE)	override { }
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override { }
    
    void Draw(class cScene *Scene) override { }
    
    void Draw(class FieldDispatcher *ffd, uint8_t transparent) override { }
    void CreateFFDData(class FieldDispatcher *rd) override { }
    void DeleteFFDData(class FieldDispatcher *rd) override { }
    
    void Draw(class ElasticSphere *es) override { }

    int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert=true) override { return 0; }
    int DeleteTexture(class cTexture *Texture) override { return 0; }
    void* LockTexture(class cTexture *Texture, int& Pitch) override { return nullptr; }
    void UnlockTexture(class cTexture *Texture) override {}

    void SetGlobalFog(const sColor4f &color,const Vect2f &v) override {};
    void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                                sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) override {};

    bool IsEnableSelfShadow() override { return false; }

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override {}

    void CreateVertexBuffer(class VertexBuffer &vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic) override;
    void DeleteVertexBuffer(class VertexBuffer &vb) override;
    void* LockVertexBuffer(class VertexBuffer &vb) override;
    void* LockVertexBuffer(class VertexBuffer &vb, uint32_t Start, uint32_t Amount) override;
    void UnlockVertexBuffer(class VertexBuffer &vb) override;
    void CreateIndexBuffer(class IndexBuffer& ib, uint32_t NumberIndices, bool dynamic) override;
    void DeleteIndexBuffer(class IndexBuffer &ib) override;
    indices_t* LockIndexBuffer(class IndexBuffer &ib) override;
    indices_t* LockIndexBuffer(class IndexBuffer &ib, uint32_t Start, uint32_t Amount) override;
    void UnlockIndexBuffer(class IndexBuffer &ib) override;
    void SubmitDrawBuffer(class DrawBuffer* db) override {}
};

#endif //PERIMETER_EMPTYRENDERDEVICE_H
