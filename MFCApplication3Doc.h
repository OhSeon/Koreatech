
// MFCApplication3Doc.h : CMFCApplication3Doc 클래스의 인터페이스
//


#pragma once
#include "stdafx.h"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib> //std::system
#include <thread>
#include <opencv2\opencv.hpp>
#include "SharedImageBuffer.h"



using std::string;

using namespace cv;
using namespace boost::interprocess;
using namespace boost::detail;

class CMFCApplication3Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMFCApplication3Doc();
	DECLARE_DYNCREATE(CMFCApplication3Doc)

// 특성입니다.
public:

// 작업입니다.
public:


// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CMFCApplication3Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
