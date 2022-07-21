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
    void SetDrawTransform(class cCamera *pDrawNode) override {}

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

    /*Внутренний мтод. Использовать с крайней осторожностью.
      Перед использованием посмотреть как используется внутри Render.
    */
    void ChangeTextColor(const char* &str,sColor4c& diffuse) override {}


    void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override { }
    void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax) override { }
    void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,
                         Vect2f uv,//Координаты текстуры в точке x,y
                         Vect2f duv,//du,dv на один логический пиксель 
            //(лог пиксель равен графическому в разрешении 1024x768)
                         float phase=0,
                         float lerp_factor=1//0..1 Насколько сильно влияет pTexture
    ) override { }


    void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255) override { }
    void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName/*="Arial"*/,int size=12,int bold=0,int italic=0,int underline=0) override { }
    bool SetScreenShot(const char *fname) override { return false; }
    int SetRenderState(eRenderStateOption option,int value) override { return -1; }
    void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,const sColor4c& Color=sColor4c(255,255,255,255)) override { }

    int Create(class cTileMap *TileMap) override { return -1; }
    void PreDraw(cTileMap *TileMap) override {}
    void Draw(cTileMap *TileMap,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow) override {}
    int Delete(class cTileMap *TileMap) override { return -1; }

    void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                            cTexture *Texture,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) override { }
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0) override { }
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE)	override { }
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override { }
    void Draw(class cScene *Scene) override { }
    
    void Draw(class FieldDispatcher *ffd) override { }
    void CreateFFDData(class FieldDispatcher *rd) override { }
    void DeleteFFDData(class FieldDispatcher *rd) override { }
    void Draw(class ElasticSphere *es) override { }

    int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,int dxout,int dyout,bool enable_assert=true) override { return 0; }
    int DeleteTexture(class cTexture *Texture) override { return 0; }
    void* LockTexture(class cTexture *Texture, int& Pitch) override { return nullptr; }
    void* LockTexture(class cTexture *Texture, int& Pitch, const Vect2i& lock_min, const Vect2i& lock_size) override { return nullptr; }
    void UnlockTexture(class cTexture *Texture) override {}

    int CreateBumpTexture(class cTexture *Texture) override { return 0; }

    void SetGlobalFog(const sColor4f &color,const Vect2f &v) override {};
    void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                                sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) override {};

    bool IsEnableSelfShadow() override { return false; }

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override {}
    void DrawIndexedPrimitive(struct sPtrVertexBuffer &vb,int OfsVertex,int nVertex,const struct sPtrIndexBuffer& ib,int nOfsPolygon,int nPolygon) override {}

    void CreateVertexBuffer(struct sPtrVertexBuffer &vb,int NumberVertex,int fmt,int dynamic=0) override;
    void DeleteVertexBuffer(struct sPtrVertexBuffer &vb) override;
    void* LockVertexBuffer(struct sPtrVertexBuffer &vb) override;
    void UnlockVertexBuffer(struct sPtrVertexBuffer &vb) override;
    void CreateIndexBuffer(struct sPtrIndexBuffer& ib,int NumberPolygon,int size=sizeof(sPolygon)) override;
    void DeleteIndexBuffer(struct sPtrIndexBuffer &ib) override;
    sPolygon* LockIndexBuffer(struct sPtrIndexBuffer &ib) override;
    void UnlockIndexBuffer(struct sPtrIndexBuffer &ib) override;
};

#endif //PERIMETER_EMPTYRENDERDEVICE_H
