// V_CSESS.CPP
// Copyright (c) A.Sobolev 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
// @codepage windows-1251
//
#include <pp.h>
#pragma hdrstop
//
// @ModuleDef(PPViewCSess)
//
IMPLEMENT_PPFILT_FACTORY(CSess); SLAPI CSessFilt::CSessFilt() : PPBaseFilt(PPFILT_CSESS, 0, 0)
{
	SetFlatChunk(offsetof(CSessFilt, ReserveStart),
		offsetof(CSessFilt, NodeList_)-offsetof(CSessFilt, ReserveStart));
	SetBranchObjIdListFilt(offsetof(CSessFilt, NodeList_));
	Init(1, 0);
}

CSessFilt & FASTCALL CSessFilt::operator = (const CSessFilt & src)
{
	Copy(&src, 1);
	return *this;
}
//
//
//
SLAPI CSessCrDraftParam::CSessCrDraftParam()
{
	Ver = 0;
	memzero(Reserve, sizeof(Reserve));
	Period.SetZero();
	RuleGrpID = RuleID = Flags = 0;
}

int SLAPI CSessCrDraftParam::Write(SBuffer & rBuf, long)
{
	int    ok = 1;
	Ver = 0;
	THROW(rBuf.Write(Ver));
	THROW(rBuf.Write(Reserve, sizeof(Reserve)));
	THROW(rBuf.Write(Period.low));
	THROW(rBuf.Write(Period.upp));
	THROW(rBuf.Write(RuleGrpID));
	THROW(rBuf.Write(RuleID));
	THROW(rBuf.Write(Flags));
	THROW(NodeList.Write(rBuf));
	CATCH
		ok = PPSetErrorSLib();
	ENDCATCH
	return ok;
}

int SLAPI CSessCrDraftParam::Read(SBuffer & rBuf, long)
{
	int    ok = 1;
	if(rBuf.GetAvailableSize()) {
		THROW(rBuf.Read(Ver));
		THROW(rBuf.Read(Reserve, sizeof(Reserve)));
		THROW(rBuf.Read(Period.low));
		THROW(rBuf.Read(Period.upp));
		THROW(rBuf.Read(RuleGrpID));
		THROW(rBuf.Read(RuleID));
		THROW(rBuf.Read(Flags));
		THROW(NodeList.Read(rBuf));
	}
	else
		ok = -1;
	CATCH
		ok = PPSetErrorSLib();
	ENDCATCH
	return ok;
}
//
//
//
#define GRP_POSNODE 1

class SelectRuleDialog : public TDialog {
public:
	SelectRuleDialog(uint dlgId) : TDialog(dlgId)
	{
		SetupCalPeriod(CTLCAL_DFRULESEL_PERIOD, CTL_DFRULESEL_PERIOD);
		addGroup(GRP_POSNODE, new PosNodeCtrlGroup(CTLSEL_DFRULESEL_NODE, cmPosNodeList));
	}
	int    setDTS(const CSessCrDraftParam * pData)
	{
		if(!RVALUEPTR(Data, pData))
			MEMSZERO(Data);
		PosNodeCtrlGroup::Rec cn_rec(&Data.NodeList);
		setGroupData(GRP_POSNODE, &cn_rec);
		SetPeriodInput(this, CTL_DFRULESEL_PERIOD, &Data.Period);
		SetupPPObjCombo(this, CTLSEL_DFRULESEL_RULEGRP, PPOBJ_DFCREATERULE, Data.RuleGrpID, 0, (void *)PPDFCRRULE_ONLYGROUPS);
		SetupPPObjCombo(this, CTLSEL_DFRULESEL_RULE, PPOBJ_DFCREATERULE, Data.RuleID, 0, (void *)PPDFCRRULE_ONLYRULES);
		AddClusterAssoc(CTL_DFRULESEL_FLAGS, 0, CSessCrDraftParam::fAllSessions);
		SetClusterData(CTL_DFRULESEL_FLAGS, Data.Flags);
		AddClusterAssoc(CTL_DFRULESEL_FLAGS2, 0, CSessCrDraftParam::fSuperSessOnly);
		SetClusterData(CTL_DFRULESEL_FLAGS2, Data.Flags);
		return 1;
	}
	int    getDTS(CSessCrDraftParam * pData)
	{
		int    ok = 1;
		uint   sel = CTL_DFRULESEL_RULE;
		PosNodeCtrlGroup::Rec cn_rec;
		THROW(getGroupData(GRP_POSNODE, &cn_rec));
		Data.NodeList = cn_rec.List;
		THROW(GetPeriodInput(this, sel = CTL_DFRULESEL_PERIOD, &Data.Period));
		Data.RuleGrpID = getCtrlLong(CTLSEL_DFRULESEL_RULEGRP);
		Data.RuleID    = getCtrlLong(CTLSEL_DFRULESEL_RULE);
		THROW_PP(Data.RuleGrpID > 0 || Data.RuleID > 0, PPERR_RULENOTSEL);
		GetClusterData(CTL_DFRULESEL_FLAGS,  &Data.Flags);
		GetClusterData(CTL_DFRULESEL_FLAGS2, &Data.Flags);
		ASSIGN_PTR(pData, Data);
		CATCH
			ok = PPErrorByDialog(this, sel);
		ENDCATCH
		return ok;
	}
private:
	DECL_HANDLE_EVENT
	{
		TDialog::handleEvent(event);
		if(event.isCbSelected(CTLSEL_DFRULESEL_RULEGRP)) {
			PPID   rule_grp = getCtrlLong(CTLSEL_DFRULESEL_RULEGRP);
			SetupPPObjCombo(this, CTLSEL_DFRULESEL_RULE, PPOBJ_DFCREATERULE, 0, 0, (void *)rule_grp);
			clearEvent(event);
		}
	}
	CSessCrDraftParam Data;
};

static int SLAPI SelectRule(CSessCrDraftParam * pData)
{
	DIALOG_PROC_BODY_P1(SelectRuleDialog, DLG_DFRULESEL, pData);
}

// static
int SLAPI PPViewCSess::EditCreateDraftParam(CSessCrDraftParam * pParam)
{
	DIALOG_PROC_BODY_P1(SelectRuleDialog, DLG_CSESSCRDRAFT, pParam);
}
//
//
//
SLAPI PPViewCSess::PPViewCSess() : PPView(0, &Filt)
{
	P_CSessIterQuery = 0;
	P_TempTbl = 0;
	P_TempOrd = 0;
	P_SessAmtAry = 0;
	CurrentViewOrder = ordByDefault;
}

SLAPI PPViewCSess::~PPViewCSess()
{
	delete P_CSessIterQuery;
	delete P_TempTbl;
	delete P_TempOrd;
	delete P_SessAmtAry;
	if(!(BaseState & bsServerInst))
		DBRemoveTempFiles();
}

PPBaseFilt * SLAPI PPViewCSess::CreateFilt(void * extraPtr) const
{
	CSessFilt * p_filt = new CSessFilt;
	if(extraPtr)
		p_filt->NodeList_.Add((long)extraPtr);
	else {
		p_filt->Flags |= CSessFilt::fOnlySuperSess;
		PPObjCashNode cn_obj;
		PPID   single_id = cn_obj.GetSingle();
		if(single_id)
			p_filt->NodeList_.Add(single_id);
	}
	return p_filt;
}
//
// ������ ������� �� �������� �������
//
int SLAPI PPViewCSess::EditBaseFilt(PPBaseFilt * pBaseFilt)
{
	class CSessFiltDialog : public TDialog {
	public:
		CSessFiltDialog() : TDialog(DLG_CSESSFILT)
		{
			addGroup(GRP_POSNODE, new PosNodeCtrlGroup(CTLSEL_CSESSFILT_NODE, cmPosNodeList));
		}
		int    setDTS(const CSessFilt * pFilt)
		{
			RVALUEPTR(Filt, pFilt);
			AddClusterAssoc(CTL_CSESSFILT_FLAGS, 0, CSessFilt::fExtBill);
			AddClusterAssoc(CTL_CSESSFILT_FLAGS, 1, CSessFilt::fOnlySuperSess);
			SetClusterData(CTL_CSESSFILT_FLAGS, Filt.Flags);
			AddClusterAssocDef(CTL_CSESSFILT_ORDER, 0, PPViewCSess::ordByDefault);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 1, PPViewCSess::ordByID);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 2, PPViewCSess::ordByDtm_CashNode);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 3, PPViewCSess::ordByDtm_CashNumber);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 4, PPViewCSess::ordByDtm_SessNumber);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 5, PPViewCSess::ordByCashNode_Dtm);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 6, PPViewCSess::ordByCashNumber_Dtm);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 7, PPViewCSess::ordBySessNumber);
			AddClusterAssoc(CTL_CSESSFILT_ORDER, 8, PPViewCSess::ordByAmount);
			SetClusterData(CTL_CSESSFILT_ORDER, Filt.InitOrder);
			SetupCalPeriod(CTLCAL_CSESSFILT_PERIOD, CTL_CSESSFILT_PERIOD);
			SetPeriodInput(this, CTL_CSESSFILT_PERIOD, &Filt.Period);
			if(Filt.NodeList_.GetCount() == 0) {
				PPObjCSession cs_obj;
				if(cs_obj.GetEqCfg().DefCashNodeID)
					Filt.NodeList_.Add(cs_obj.GetEqCfg().DefCashNodeID);
			}
			{
				PosNodeCtrlGroup::Rec cn_rec(&Filt.NodeList_);
				setGroupData(GRP_POSNODE, &cn_rec);
			}
			setCtrlData(CTL_CSESSFILT_CASHN, &Filt.CashNumber);
			return 1;
		}
		int    getDTS(CSessFilt * pFilt)
		{
			int    ok = 1;
			uint   sel = 0;
			GetClusterData(CTL_CSESSFILT_FLAGS, &Filt.Flags);
			GetClusterData(CTL_CSESSFILT_ORDER, &Filt.InitOrder);
			GetPeriodInput(this, sel = CTL_CSESSFILT_PERIOD, &Filt.Period);
			PosNodeCtrlGroup::Rec cn_rec;
			THROW(ObjRts.AdjustCSessPeriod(Filt.Period, 1)); // @v9.2.11
			THROW(getGroupData(sel = GRP_POSNODE, &cn_rec));
			Filt.NodeList_ = cn_rec.List;
			getCtrlData(CTL_CSESSFILT_CASHN, &Filt.CashNumber);
			ASSIGN_PTR(pFilt, Filt);
			CATCH
				ok = PPErrorByDialog(this, sel);
			ENDCATCH
			return ok;
		}
	private:
		CSessFilt  Filt;
	};
	if(!Filt.IsA(pBaseFilt))
		return 0;
	CSessFilt * p_filt = (CSessFilt *)pBaseFilt;
	DIALOG_PROC_BODY(CSessFiltDialog, p_filt);
}
//
//
//
PP_CREATE_TEMP_FILE_PROC(CreateTempCSessFile, TempCSessChecks);

int SLAPI PPViewCSess::Init_(const PPBaseFilt * pBaseFilt)
{
	int    ok = 1;
	const PPRights & r_rt = ObjRts;
	THROW(CsObj.CheckRights(PPR_READ));
	THROW(Helper_InitBaseFilt(pBaseFilt));
	PPWait(1);
	ZDELETE(P_TempTbl);
	ZDELETE(P_TempOrd);
	Filt.Period.Actualize(ZERODATE);
	THROW(r_rt.AdjustCSessPeriod(Filt.Period, 0));
	CurrentViewOrder = Filt.InitOrder;
	{

		NodeList.Set(0);
		ObjIdListFilt temp_node_list;
		if(Filt.NodeList_.GetCount())
			temp_node_list = Filt.NodeList_;
		else if(r_rt.P_PosList && r_rt.P_PosList->getCount()) {
			for(uint i = 0; i < r_rt.P_PosList->getCount(); i++) {
				temp_node_list.Add(r_rt.P_PosList->at(i).ObjID);
			}
		}
		if(temp_node_list.GetCount()) {
			PPObjCashNode cn_obj;
			PPCashNode2 cn_rec;
			PPIDArray temp_list;
			for(uint i = 0; i < temp_node_list.GetCount(); i++) {
				PPID   cn_id = temp_node_list.Get().get(i);
				if(cn_obj.Fetch(cn_id, &cn_rec) > 0 && r_rt.CheckPosNodeID(cn_id, 0)) {
					if(cn_rec.CashType == PPCMT_CASHNGROUP) {
						temp_list.clear();
						cn_obj.GetListByGroup(cn_id, temp_list);
						for(uint j = 0; j < temp_list.getCount(); j++) {
							const PPID inner_cn_id = temp_list.get(j);
							PPCashNode inner_cn_rec;
							if(inner_cn_id && cn_obj.Fetch(inner_cn_id, &inner_cn_rec) > 0 && r_rt.CheckPosNodeID(inner_cn_id, 0))
								NodeList.Add(inner_cn_id);
						}
					}
					NodeList.Add(cn_id);
				}
			}
		}
	}
	if(Filt.Flags & CSessFilt::fExtBill) {
		THROW(P_TempTbl = CreateTempCSessFile());
		{
			BExtInsert bei(P_TempTbl);
			PPTransaction tra(ppDbDependTransaction, 1);
			THROW(tra);
			if(InitCSessIteration()) {
				CSessionTbl::Rec csess_rec;
				for(; NextCSessIteration(&csess_rec) > 0; Counter.Increment()) {
					THROW(Add(&bei, &csess_rec) > 0);
					PPWaitPercent(Counter);
				}
			}
			THROW_DB(bei.flash());
			THROW(tra.Commit());
		}
	}
	PPWait(0);
	CATCH
		ZDELETE(P_TempTbl);
		ZDELETE(P_TempOrd);
		ok = 0;
	ENDCATCH
	return ok;
}

int SLAPI PPViewCSess::CreateOrderTable(long ord, TempOrderTbl ** ppTbl)
{
	int    ok = 1;
	TempOrderTbl * p_o = 0;
	*ppTbl = 0;
	if(IsNotDefaultOrder(ord)) {
		CSessViewItem item;
		THROW(p_o = CreateTempOrderFile());
		{
			SString temp_buf;
			PPObjCashNode cn_obj;
			PPCashNode cn_rec;
			BExtInsert bei(p_o);
			PPIDArray id_list; // @debug
			for(InitIteration(ordByDefault); NextIteration(&item) > 0;) {
				const double large_val = 1e12;
				TempOrderTbl::Rec ord_rec;
				MEMSZERO(ord_rec);
				ord_rec.ID = item.ID;
				if(id_list.addUnique(ord_rec.ID) < 0) {
					temp_buf = "debug";
				}
				temp_buf = 0;
				LDATETIME dtm;
				dtm.Set(item.Dt, item.Tm);
				switch(ord) {
					case ordByID:
						temp_buf.CatLongZ(item.ID, 20);
						break;
					case ordByDtm_CashNode:
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						if(cn_obj.Fetch(item.CashNodeID, &cn_rec) > 0)
							temp_buf.Cat(cn_rec.Name);
						else
							temp_buf.CatLongZ(item.CashNodeID, 20);
						break;
					case ordByDtm_CashNumber:
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						temp_buf.CatLongZ(item.CashNumber, 20);
						break;
					case ordByDtm_SessNumber:
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						temp_buf.CatLongZ(item.SessNumber, 20);
						break;
					case ordByCashNode_Dtm:
						if(cn_obj.Fetch(item.CashNodeID, &cn_rec) > 0)
							temp_buf.Cat(cn_rec.Name);
						else
							temp_buf.CatLongZ(item.CashNodeID, 20);
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						break;
					case ordByCashNumber_Dtm:
						temp_buf.CatLongZ(item.CashNumber, 20);
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						break;
					case ordBySessNumber:
						temp_buf.CatLongZ(item.SessNumber, 20);
						temp_buf.Cat(dtm, DATF_YMD|DATF_CENTURY, TIMF_HMS|TIMF_MSEC);
						break;
					case ordByAmount:
						temp_buf.Cat(large_val-item.Amount, MKSFMTD(30, 5, 0));
						break;
				};
				temp_buf.CopyTo(ord_rec.Name, sizeof(ord_rec.Name));
				THROW_DB(bei.insert(&ord_rec));
			}
			THROW_DB(bei.flash());
		}
		*ppTbl = p_o;
		p_o = 0;
	}
	else
		ok = -1;
	CATCHZOK
	delete p_o;
	return ok;
}

int SLAPI PPViewCSess::InitIteration(long ord)
{
	int    ok = 1, r;
	Counter.Init();
	BExtQuery::ZDelete(&P_IterQuery);
	{
		TempOrderTbl * p_temp_ord = 0;
		THROW(r = CreateOrderTable(ord, &p_temp_ord));
		if(r > 0) {
			DELETEANDASSIGN(P_TempOrd, p_temp_ord);
		}
	}
	if(P_TempOrd) {
		TempOrderTbl::Key1 k1;
		THROW_MEM(P_IterQuery = new BExtQuery(P_TempOrd, 1));
		P_IterQuery->select(P_TempOrd->ID, 0);
		Counter.Init(P_IterQuery->countIterations(0, MEMSZERO(k1), spFirst));
		P_IterQuery->initIteration(0, MEMSZERO(k1), spFirst);
	}
	else {
		DBQ  * dbq = 0;
		int    idx = 0;
		union {
			CSessionTbl::Key0 k0;
			CSessionTbl::Key1 k1;
			CSessionTbl::Key2 k2;
			CSessionTbl::Key3 k3;
		} k, k_;
		MEMSZERO(k);
		const  PPID cn_id = NodeList.GetSingle();
		if(Filt.SuperSessID) {
			idx = 3;
			k.k3.SuperSessID = Filt.SuperSessID;
			k.k3.CashNumber = Filt.CashNumber;
			k.k3.Dt = Filt.Period.low;
		}
		else if(cn_id)
			if(Filt.CashNumber) {
				idx = 2;
				k.k2.CashNodeID = cn_id;
				k.k2.CashNumber = Filt.CashNumber;
			}
			else {
				idx = 1;
				k.k1.CashNodeID = cn_id;
				k.k1.Dt = Filt.Period.low;
			}
		else {
			idx = 0;
			k.k0.ID = 0;
		}
		if(Filt.Flags & CSessFilt::fExtBill) {
			THROW_MEM(P_IterQuery = new BExtQuery(P_TempTbl, idx));
			P_IterQuery->selectAll();
		}
		else {
			CSessionTbl * t = CsObj.P_Tbl;
			THROW_MEM(P_IterQuery = new BExtQuery(t, idx));
			P_IterQuery->selectAll();
			if(Filt.SuperSessID)
				dbq = &(t->SuperSessID == Filt.SuperSessID);
			else {
				if(Filt.Flags & CSessFilt::fOnlySuperSess)
					dbq = &(*dbq && t->SuperSessID == 0L);
				if(cn_id)
					dbq = &(*dbq && t->CashNodeID == cn_id);
			}
			if(Filt.CashNumber)
				dbq = &(*dbq && t->CashNumber == Filt.CashNumber);
			dbq = &(*dbq && daterange(t->Dt, &Filt.Period));
			if(NodeList.IsExists())
				dbq = ppcheckfiltidlist(dbq, t->CashNodeID, &NodeList.Get());
			P_IterQuery->where(*dbq);
		}
		k_ = k;
		Counter.Init(P_IterQuery->countIterations(0, &k_, spGt));
		P_IterQuery->initIteration(0, &k, spGt);
	}
	CATCHZOK
	return ok;
}

int FASTCALL PPViewCSess::NextIteration(CSessViewItem * pItem)
{
	int    ok = -1;
	while(ok < 0 && P_IterQuery && P_IterQuery->nextIteration() > 0) {
		Counter.Increment();
		if(P_TempOrd) {
			PPID   id = P_TempOrd->data.ID;
			if(P_TempTbl) {
				TempCSessChecksTbl::Rec temp_rec;
				if(SearchByKey(P_TempTbl, 0, &id, &temp_rec) > 0) {
					memcpy(pItem, &temp_rec, sizeof(CSessionTbl::Rec));
					pItem->ChkCount       = temp_rec.ChkCount;
					pItem->WORetAmount    = temp_rec.WORetAmount;
					pItem->WORetBnkAmount = temp_rec.WORetBnkAmount;
					pItem->BnkDiscount    = temp_rec.BnkDiscount;
					ok = 1;
				}
			}
			else if(CsObj.Search(id, pItem) > 0) {
				pItem->ChkCount       = 0;
				pItem->WORetAmount    = 0;
				pItem->WORetBnkAmount = 0;
				pItem->BnkDiscount    = 0;
				ok = 1;
			}
		}
		else {
			if(Filt.Flags & CSessFilt::fExtBill && P_TempTbl) {
				memcpy(pItem, &P_TempTbl->data, sizeof(CSessionTbl::Rec));
				pItem->ChkCount       = P_TempTbl->data.ChkCount;
				pItem->WORetAmount    = P_TempTbl->data.WORetAmount;
				pItem->WORetBnkAmount = P_TempTbl->data.WORetBnkAmount;
				pItem->BnkDiscount    = P_TempTbl->data.BnkDiscount;
			}
			else {
				CsObj.P_Tbl->copyBufTo(pItem);
				pItem->ChkCount       = 0;
				pItem->WORetAmount    = 0;
				pItem->WORetBnkAmount = 0;
				pItem->BnkDiscount    = 0;
			}
			ok = 1;
		}
	}
	return ok;
}

DBQuery * SLAPI PPViewCSess::CreateBrowserQuery(uint * pBrwId, SString * pSubTitle)
{
	uint   brw_id = (Filt.Flags & CSessFilt::fExtBill) ? BROWSER_CSESS_EXT : BROWSER_CSESS;
	DBQuery * p_q = 0;
	DBQ  * dbq = 0;
	DBE    dbe_cashnode;
	DBE  * p_dbe_ret = 0;
	CSessionTbl * t = 0;
	TempCSessChecksTbl * p_ch = 0;
	TempOrderTbl * p_ot = 0;
	const  PPID cn_id = NodeList.GetSingle();
	THROW(CreateOrderTable(CurrentViewOrder, &p_ot));
	if(Filt.Flags & CSessFilt::fExtBill) {
		THROW(P_CSessIterQuery);
		THROW(p_ch = new TempCSessChecksTbl(P_TempTbl->fileName));
		PPDbqFuncPool::InitObjNameFunc(dbe_cashnode, PPDbqFuncPool::IdObjNameCashNode, p_ch->CashNodeID);
		p_dbe_ret = &(p_ch->WORetAmount-p_ch->Amount); // �������������� ����������� � ����������, ����� ��������
			// ������������� �������� ���������
		p_q = &select(
			p_ch->ID,             // #0
			p_ch->Dt,             // #1
			p_ch->SessNumber,     // #2
			p_ch->CashNumber,     // #3
			p_ch->Incomplete,     // #4
			p_ch->Amount,         // #5
			p_ch->Discount,       // #6
			p_ch->WrOffAmount,    // #7
			p_ch->AggrRest,       // #8
			p_ch->WrOffCost,      // #9
			p_ch->Income,         // #10
			p_ch->BnkAmount,      // #11
			p_ch->ChkCount,       // #12
			p_ch->WORetAmount,    // #13
			*p_dbe_ret,           // #14
			p_ch->BnkDiscount,    // #15
			dbe_cashnode,         // #16
			p_ch->CSCardAmount,   // #17
			0L);
		if(p_ot) {
			p_q->from(p_ot, p_ch, 0L).where(p_ch->ID == p_ot->ID).orderBy(p_ot->Name, 0L);
		}
		else {
			p_q->from(p_ch, 0L);
			dbq = &(*dbq && daterange(p_ch->Dt, &Filt.Period));
			p_q->where(*dbq);
			if(Filt.SuperSessID)
				p_q->orderBy(p_ch->SuperSessID, p_ch->CashNumber, p_ch->Dt, 0L);
			else if(cn_id)
				if(Filt.CashNumber)
					p_q->orderBy(p_ch->CashNodeID, p_ch->CashNumber, 0L);
				else
					p_q->orderBy(p_ch->CashNodeID, p_ch->Dt, 0L);
			else
				p_q->orderBy(p_ch->ID, 0L);
		}
	}
	else {
		THROW_MEM(t = new CSessionTbl);
		PPDbqFuncPool::InitObjNameFunc(dbe_cashnode, PPDbqFuncPool::IdObjNameCashNode, t->CashNodeID);
		p_q = &select(
			t->ID,           // #00
			t->Dt,           // #01
			t->SessNumber,   // #02
			t->CashNumber,   // #03
			t->Incomplete,   // #04
			t->Amount,       // #05
			t->Discount,     // #06
			t->WrOffAmount,  // #07
			t->AggrRest,     // #08
			t->BnkAmount,    // #09
			dbe_cashnode,    // #10
			t->CSCardAmount, // #11
			0L);
		if(p_ot) {
			p_q->from(p_ot, t, 0L);
			dbq = &(t->ID == p_ot->ID);
			/*
			if(Filt.SuperSessID)
				dbq = &(*dbq && t->SuperSessID == Filt.SuperSessID);
			else {
				if(Filt.Flags & CSessFilt::fOnlySuperSess)
					dbq = &(*dbq && t->SuperSessID == 0L);
				dbq = ppcheckfiltid(dbq, t->CashNodeID, cn_id);
			}
			dbq = ppcheckfiltid(dbq, t->CashNumber, Filt.CashNumber);
			dbq = &(*dbq && daterange(t->Dt, &Filt.Period));
			if(Filt.NodeList.IsExists())
				dbq = ppcheckfiltidlist(dbq, t->CashNodeID, &Filt.NodeList.Get());
			*/
			p_q->where(*dbq).orderBy(p_ot->Name, 0L);
		}
		else {
			p_q->from(t, 0L);
			if(Filt.SuperSessID)
				dbq = &(t->SuperSessID == Filt.SuperSessID);
			else {
				if(Filt.Flags & CSessFilt::fOnlySuperSess)
					dbq = &(*dbq && t->SuperSessID == 0L);
				dbq = ppcheckfiltid(dbq, t->CashNodeID, cn_id);
			}
			dbq = ppcheckfiltid(dbq, t->CashNumber, Filt.CashNumber);
			dbq = &(*dbq && daterange(t->Dt, &Filt.Period));
			if(NodeList.IsExists())
				dbq = ppcheckfiltidlist(dbq, t->CashNodeID, &NodeList.Get());
			p_q->where(*dbq);
			if(Filt.SuperSessID)
				p_q->orderBy(t->SuperSessID, t->CashNumber, t->Dt, 0L);
			else if(cn_id)
				if(Filt.CashNumber)
					p_q->orderBy(t->CashNodeID, t->CashNumber, 0L);
				else
					p_q->orderBy(t->CashNodeID, t->Dt, 0L);
			else
				p_q->orderBy(t->Dt, t->Tm, 0L);
		}
	}
	if(pSubTitle) {
		*pSubTitle = 0;
		if(NodeList.GetCount()) {
			SString temp_buf;
			for(uint i = 0; i < NodeList.GetCount(); i++) {
				if(GetObjectName(PPOBJ_CASHNODE, NodeList.Get().get(i), temp_buf) > 0)
					pSubTitle->CatDivIfNotEmpty(';', 2).Cat(temp_buf);
				if(pSubTitle->Len() > 64 && i < NodeList.GetCount()-1) {
					pSubTitle->CatCharN('.', 2);
					break;
				}
			}
		}
	}
	CATCH
		if(p_q)
			ZDELETE(p_q);
		else {
			delete t;
			delete p_ch;
			delete p_ot;
		}
	ENDCATCH
	delete p_dbe_ret;
	ASSIGN_PTR(pBrwId, brw_id);
	return p_q;
}

int SLAPI PPViewCSess::ProcessCommand(uint ppvCmd, const void * pHdr, PPViewBrowser * pBrw)
{
	int    ok = PPView::ProcessCommand(ppvCmd, pHdr, pBrw);
	if(ok == -2) {
		PPID   id = pHdr ? *(PPID *)pHdr : 0;
		switch(ppvCmd) {
			case PPVCMD_ADDITEM:
				ok = AddItem();
				break;
			case PPVCMD_EDITITEMSPC:
				ok = EditItem(id);
				break;
			case PPVCMD_DELETEITEM:
				ok = DeleteItem(id);
				break;
			case PPVCMD_VIEWDEFICIT:
				ok = ViewExcesses(id);
				break;
			case PPVCMD_VIEWGOODSOPANLZ:
				ok = -1;
				ViewGoodsOpAnlz(id);
				break;
			case PPVCMD_VIEWGOODSTAXANLZ:
				ok = -1;
				ViewGoodsTaxAnlz(id);
				break;
			case PPVCMD_WROFFBILLS:
				ok = ViewBillsByPool(PPASS_CSESSBILLPOOL, id);
				break;
			case PPVCMD_WROFFDRAFT:
				ok = -1;
				if(id) {
					PPIDArray list;
					list.add(id);
					WriteOffDrafts(&list);
				}
				break;
			case PPVCMD_DFCTBILLS:
				ok = ViewBillsByPool(PPASS_CSDBILLPOOL, id);
				break;
			case PPVCMD_WROFFBILLSALL:
				ok = -1;
				{
					// PPASS_CSESSBILLPOOL
					MemLeakTracer mlt;
					BillFilt flt;
					if(GetBillList(0, flt.List) > 0 && !flt.List.IsEmpty())
						::ViewGoodsBills(&flt, /*1*/0);
				}
				break;
			case PPVCMD_CCHECKS:
				ok = ViewAllChecks();
				break;
			case PPVCMD_CLOSESESS:
				ok = CloseSession();
				break;
			case PPVCMD_WROFFSESS:
				ok = CompleteSession(id);
				break;
			case PPVCMD_DORECOVER:
				ok = RecalcSession(id);
				break;
			case PPVCMD_DETACH:
				ok = DetachSessFromSuperSess(id);
				break;
			case PPVCMD_TRANSMIT:
				ok = -1;
				Transmit(id, 0);
				break;
			case PPVCMD_CREATEDRAFT:
				ok = -1;
				{
					int    all_sess = 0;
					CSessCrDraftParam item;
					MEMSZERO(item);
					if(SelectRule(&item) > 0)
						CreateDrafts(item.RuleGrpID, item.RuleID, (item.Flags & CSessCrDraftParam::fAllSessions) ? 0 : id);
				}
				break;
			case PPVCMD_PRINTITEM:
				ok = -1;
				PrintSession(id);
				break;
			case PPVCMD_PRINTCHECK:
				ok = -1;
				PosPrint(id);
				break;
			case PPVCMD_EXPORT:
				Transmit(id, 1);
				break;
			case PPVCMD_SYSJ:
				if(id)
					ViewSysJournal(PPOBJ_CSESSION, id, 0);
				break;
		}
	}
	return ok;
}

int SLAPI PPViewCSess::InitCSessIteration()
{
	int    ok = 1;
	int    idx = 0;
	DBQ  * dbq = 0;
	CSessionTbl * t = CsObj.P_Tbl;
	union {
		CSessionTbl::Key0 k0;
		CSessionTbl::Key1 k1;
		CSessionTbl::Key2 k2;
		CSessionTbl::Key3 k3;
	} k, ck;
	MEMSZERO(k);
	const PPID cn_id = NodeList.GetSingle();
	if(Filt.SuperSessID) {
		idx = 3;
		k.k3.SuperSessID = Filt.SuperSessID;
		k.k3.CashNumber = Filt.CashNumber;
		k.k3.Dt = Filt.Period.low;
	}
	else if(cn_id)
		if(Filt.CashNumber) {
			idx = 2;
			k.k2.CashNodeID = cn_id;
			k.k2.CashNumber = Filt.CashNumber;
		}
		else {
			idx = 1;
			k.k1.CashNodeID = cn_id;
			k.k1.Dt = Filt.Period.low;
		}
	else {
		idx = 0;
		k.k0.ID = 0;
	}
	ZDELETE(P_CSessIterQuery);
	THROW_MEM(P_CSessIterQuery = new BExtQuery(t, idx));
	P_CSessIterQuery->selectAll();
	if(Filt.SuperSessID)
		dbq = &(*dbq && t->SuperSessID == Filt.SuperSessID);
	else {
		if(Filt.Flags & CSessFilt::fOnlySuperSess)
			dbq = &(*dbq && t->SuperSessID == 0L);
		dbq = ppcheckfiltid(dbq, t->CashNodeID, cn_id);
	}
	dbq = ppcheckfiltid(dbq, t->CashNumber, Filt.CashNumber);
	dbq = &(*dbq && daterange(t->Dt, &Filt.Period));
	if(NodeList.IsExists())
		dbq = ppcheckfiltidlist(dbq, t->CashNodeID, &NodeList.Get());
	P_CSessIterQuery->where(*dbq);
	ck = k;
	Counter.Init(P_CSessIterQuery->countIterations(0, &ck, spGt));
	P_CSessIterQuery->initIteration(0, &k, spGt);
	CATCHZOK
	return ok;
}

int SLAPI PPViewCSess::NextCSessIteration(CSessionTbl::Rec * pRec)
{
	int    ok = -1;
	if(P_CSessIterQuery && P_CSessIterQuery->nextIteration() > 0) {
		CSessionTbl::Rec csess_rec;
		CsObj.P_Tbl->copyBufTo(&csess_rec);
		ASSIGN_PTR(pRec, csess_rec);
		ok = 1;
	}
	return ok;
}

struct SessAmtEntry {
	PPID   SessID;
	long   ChkCount;
	double WORetAmount;
	double WORetBnkAmount;
	double BnkDiscount;
};

int SLAPI PPViewCSess::CalcCheckAmounts(TempCSessChecksTbl::Rec * pRec)
{
	int    ok = -1;
	if(pRec) {
		CSessionTbl::Rec sess_rec;
		pRec->ChkCount       = 0;
		pRec->WORetAmount    = 0.0;
		pRec->WORetBnkAmount = 0.0;
		pRec->BnkDiscount    = 0.0;
		if(pRec->ID && CsObj.Search(pRec->ID, &sess_rec) > 0) {
			uint  i;
			PPIDArray    sess_list;
			THROW_MEM(SETIFZ(P_SessAmtAry, new SArray(sizeof(SessAmtEntry))));
			if(!pRec->SuperSessID)
				CsObj.P_Tbl->GetSubSessList(pRec->ID, &sess_list);
			if(!sess_list.getCount())
				sess_list.addUnique(pRec->ID);
			for(i = 0; i < sess_list.getCount(); i++) {
				uint  pos = 0;
				PPID  sess_id = sess_list.at(i);
				SessAmtEntry sa_entry;
				if(P_SessAmtAry->lsearch(&sess_id, &pos, CMPF_LONG))
					sa_entry = *(SessAmtEntry *)P_SessAmtAry->at(pos);
				else {
					CSessTotal  cs_total;
					CC.GetSessTotal(sess_id, CsObj.P_Tbl->GetCcGroupingFlags(sess_rec, sess_id), &cs_total, 0);
					sa_entry.SessID         = sess_id;
					sa_entry.ChkCount       = cs_total.CheckCount;
					sa_entry.WORetAmount    = cs_total.WORetAmount;
					sa_entry.WORetBnkAmount = cs_total.WORetBnkAmount;
					sa_entry.BnkDiscount    = cs_total.BnkDiscount;
					THROW(P_SessAmtAry->insert(&sa_entry));
				}
				pRec->ChkCount       += sa_entry.ChkCount;
				pRec->WORetAmount    += sa_entry.WORetAmount;
				pRec->WORetBnkAmount += sa_entry.WORetBnkAmount;
				pRec->BnkDiscount    += sa_entry.BnkDiscount;
			}
			ok = 1;
		}
	}
	CATCHZOK
	return ok;
}

int SLAPI PPViewCSess::Add(BExtInsert * pBei, CSessionTbl::Rec * pRec)
{
	int    ok = 1;
	TempCSessChecksTbl::Rec csch_rec;
	MEMSZERO(csch_rec);
	csch_rec.ID          = pRec->ID;
	csch_rec.SuperSessID = pRec->SuperSessID;
	csch_rec.CashNodeID  = pRec->CashNodeID;
	csch_rec.CashNumber  = pRec->CashNumber;
	csch_rec.SessNumber  = pRec->SessNumber;
	csch_rec.Dt          = pRec->Dt;
	csch_rec.Tm          = pRec->Tm;
	csch_rec.Incomplete  = pRec->Incomplete;
	csch_rec.Amount      = pRec->Amount;
	csch_rec.Discount    = pRec->Discount;
	csch_rec.AggrAmount  = pRec->AggrAmount;
	csch_rec.AggrRest    = pRec->AggrRest;
	csch_rec.WrOffAmount = pRec->WrOffAmount;
	csch_rec.BnkAmount   = pRec->BnkAmount;     // @CSCardAmount
	csch_rec.CSCardAmount = pRec->CSCardAmount;
	if(BillObj->CheckRights(BILLRT_ACCSCOST)) {
		const  PPID ret_op_id = GetCashRetOp();
		double cost_amount = 0.0;
		PPIDArray bill_list;
		BillObj->P_Tbl->GetPoolMembersList(PPASS_CSESSBILLPOOL, csch_rec.ID, &bill_list);
		for(uint i = 0; i < bill_list.getCount(); i++) {
			const PPID member_id = bill_list.get(i);
			BillTbl::Rec bill_rec;
			if(BillObj->Fetch(member_id, &bill_rec) > 0) {
				double amt = BR2(bill_rec.Amount);
				BillObj->P_Tbl->GetAmount(member_id, PPAMT_BUYING, 0, &amt);
				if(bill_rec.OpID == ret_op_id)
					amt = -amt;
				cost_amount += amt;
			}
		}
		csch_rec.WrOffCost = cost_amount;
		csch_rec.Income    = csch_rec.WrOffAmount - csch_rec.WrOffCost;
	}
	else {
		csch_rec.WrOffCost = 0.0;
		csch_rec.Income    = 0.0;
	}
	THROW(CalcCheckAmounts(&csch_rec));
	THROW_DB(pBei->insert(&csch_rec));
	CATCHZOK
	return ok;
}

// AHTOXA {
class DraftCreateRuleDialog : public TDialog {
public:
	DraftCreateRuleDialog() : TDialog(DLG_DFRULE)
	{
		Data.Init();
	}
	int    setDTS(const PPDfCreateRulePacket *);
	int    getDTS(PPDfCreateRulePacket *);
private:
	DECL_HANDLE_EVENT;
	int    setupCtrls();

	PPDfCreateRulePacket Data;
};

IMPL_HANDLE_EVENT(DraftCreateRuleDialog)
{
	TDialog::handleEvent(event);
	if(event.isCbSelected(CTLSEL_DFRULE_PQUOT)) {
		getCtrlData(CTLSEL_DFRULE_PQUOT, &Data.Rec.PQuot);
		setupCtrls();
	}
	else if(event.isCbSelected(CTLSEL_DFRULE_CQUOT)) {
		getCtrlData(CTLSEL_DFRULE_CQUOT, &Data.Rec.CQuot);
		setupCtrls();
	}
	else if(event.isCbSelected(CTLSEL_DFRULE_OPKIND)) {
		PPOprKind op_kind;
		getCtrlData(CTLSEL_DFRULE_OPKIND, &Data.Rec.OpID);
		GetOpData(Data.Rec.OpID, &op_kind);
		SetupArCombo(this, CTLSEL_DFRULE_OBJECT,  0, 0, op_kind.AccSheetID, sacfDisableIfZeroSheet);
	}
	else if(event.isCbSelected(CTLSEL_DFRULE_GGRP)) {
		getCtrlData(CTLSEL_DFRULE_GGRP, &Data.Rec.GoodsGrpID);
		setupCtrls();
	}
	else if(event.isCbSelected(CTLSEL_DFRULE_SCARDSER))
		setupCtrls();
	else if(event.isClusterClk(CTL_DFRULE_COSTALG) || event.isClusterClk(CTL_DFRULE_PRICEALG) || event.isClusterClk(CTL_DFRULE_WOSCARD)) {
		setupCtrls();
	}
	else
		return;
	clearEvent(event);
}

int DraftCreateRuleDialog::setupCtrls()
{
	int    disable_all = BIN(Data.Rec.Flags & PPDraftCreateRule::fIsRulesGroup);
	ushort v = getCtrlUInt16(CTL_DFRULE_PRICEALG);
	disableCtrl(CTL_DFRULE_PPCTVAL, v != 3 || disable_all);
	ushort v1 = getCtrlUInt16(CTL_DFRULE_COSTALG);
	long   lv = 0, wo_scard_ser = 0;

	GetClusterData(CTL_DFRULE_WOSCARD, &lv);
	getCtrlData(CTLSEL_DFRULE_SCARDSER, &Data.Rec.SCardSerID);
	wo_scard_ser = BIN(lv & PPDraftCreateRule::fWoSCard);
	disableCtrl(CTL_DFRULE_CPCTVAL, v == 3 || v1 != 1 || disable_all);
	disableCtrl(CTL_DFRULE_COSTALG,  v == 3 || Data.Rec.CQuot || disable_all);
	disableCtrl(CTL_DFRULE_PRICEALG, Data.Rec.PQuot || disable_all);
	disableCtrl(CTLSEL_DFRULE_PQUOT, v == 3 || disable_all);
	if(v == 3)
		setCtrlUInt16(CTL_DFRULE_COSTALG, 0);
	disableCtrl(CTL_DFRULE_EXCLGGRP, !Data.Rec.GoodsGrpID || disable_all);
	if(!Data.Rec.GoodsGrpID || disable_all)
		Data.Rec.Flags &= ~PPDraftCreateRule::fExclGoodsGrp;
	if(disable_all)
		disableCtrls(1, CTLSEL_DFRULE_OPKIND, CTLSEL_DFRULE_OBJECT, CTLSEL_DFRULE_AGENT,
			CTLSEL_DFRULE_CQUOT, CTL_DFRULE_CASHNN, CTLSEL_DFRULE_GGRP, CTLSEL_DFRULE_RULEGRP,
			CTL_DFRULE_PRICEALG, CTL_DFRULE_COSTALG, CTL_DFRULE_MAXPOS, CTL_DFRULE_MAXSUM, 0L);
	if(wo_scard_ser || !Data.Rec.SCardSerID) {
		lv &= ~PPDraftCreateRule::fExcludeSCardSer;
		SetClusterData(CTL_DFRULE_WOSCARD, lv);
		setCtrlLong(CTLSEL_DFRULE_SCARDSER, 0L);
	}
	disableCtrl(CTLSEL_DFRULE_SCARDSER, wo_scard_ser);
	DisableClusterItem(CTL_DFRULE_WOSCARD, 1, wo_scard_ser || !Data.Rec.SCardSerID);
	return 1;
}

int DraftCreateRuleDialog::setDTS(const PPDfCreateRulePacket * pData)
{
	ushort v = 0;
	SString cash_nn;
	PPIDArray types;
	PPOprKind op_kind;
	if(!RVALUEPTR(Data, pData))
		Data.Init();
	setCtrlData(CTL_DFRULE_NAME, Data.Rec.Name);
	if(!(Data.Rec.Flags & PPDraftCreateRule::fIsRulesGroup)) {
		setCtrlData(CTL_DFRULE_CPCTVAL, &Data.Rec.CPctVal);
		setCtrlData(CTL_DFRULE_PPCTVAL, &Data.Rec.PPctVal);
		Data.GetCashNN(&cash_nn);
		setCtrlString(CTL_DFRULE_CASHNN, cash_nn);

		types.add(PPOPT_DRAFTRECEIPT);
		types.add(PPOPT_DRAFTEXPEND);
		SetupOprKindCombo(this, CTLSEL_DFRULE_OPKIND, Data.Rec.OpID, 0, &types, 0);
		GetOpData(Data.Rec.OpID, &op_kind);
		SetupArCombo(this, CTLSEL_DFRULE_OBJECT,  Data.Rec.ArID,  0, op_kind.AccSheetID, sacfDisableIfZeroSheet);
		SetupArCombo(this, CTLSEL_DFRULE_AGENT, Data.Rec.AgentID, OLW_CANINSERT, GetAgentAccSheet(), sacfDisableIfZeroSheet);
		SetupPPObjCombo(this, CTLSEL_DFRULE_GGRP,  PPOBJ_GOODSGROUP, Data.Rec.GoodsGrpID, OLW_CANSELUPLEVEL);
		SetupPPObjCombo(this, CTLSEL_DFRULE_CQUOT, PPOBJ_QUOTKIND, Data.Rec.CQuot, 0);
		SetupPPObjCombo(this, CTLSEL_DFRULE_PQUOT, PPOBJ_QUOTKIND, Data.Rec.PQuot, 0);
		SetupPPObjCombo(this, CTLSEL_DFRULE_RULEGRP, PPOBJ_DFCREATERULE, Data.Rec.ParentID, 0, (void *)PPDFCRRULE_ONLYGROUPS);

		SETFLAG(v, 0x01, Data.Rec.Flags & PPDraftCreateRule::fExclGoodsGrp);
		setCtrlData(CTL_DFRULE_EXCLGGRP, &v);

		AddClusterAssocDef(CTL_DFRULE_PRICEALG,  0, PPDraftCreateRule::pByLastLot);
		AddClusterAssoc(CTL_DFRULE_PRICEALG,  1, PPDraftCreateRule::pByAvgSum);
		AddClusterAssoc(CTL_DFRULE_PRICEALG,  2, PPDraftCreateRule::pByAvgSumDis);
		AddClusterAssoc(CTL_DFRULE_PRICEALG,  3, PPDraftCreateRule::pByCostPctVal);
		SetClusterData(CTL_DFRULE_PRICEALG, Data.Rec.PriceAlg);

		AddClusterAssocDef(CTL_DFRULE_COSTALG,  0, PPDraftCreateRule::cByLastLot);
		AddClusterAssoc(CTL_DFRULE_COSTALG,  1, PPDraftCreateRule::cByPricePctVal);
		SetClusterData(CTL_DFRULE_COSTALG, Data.Rec.CostAlg);

		setCtrlData(CTL_DFRULE_MAXPOS, &Data.Rec.MaxPos);
		setCtrlData(CTL_DFRULE_MAXSUM, &Data.Rec.MaxSum);

		AddClusterAssocDef(CTL_DFRULE_PAYTYPE,  0, 0);
		AddClusterAssoc(CTL_DFRULE_PAYTYPE,  1, PPDraftCreateRule::fOnlyBanking);
		AddClusterAssoc(CTL_DFRULE_PAYTYPE,  2, PPDraftCreateRule::fOnlyNotBanking);
		long pay_type = (Data.Rec.Flags & (PPDraftCreateRule::fOnlyBanking|PPDraftCreateRule::fOnlyNotBanking));
		SetClusterData(CTL_DFRULE_PAYTYPE, pay_type);

		AddClusterAssoc(CTL_DFRULE_WOSCARD, 0, PPDraftCreateRule::fWoSCard);
		AddClusterAssoc(CTL_DFRULE_WOSCARD, 1, PPDraftCreateRule::fExcludeSCardSer);
		AddClusterAssoc(CTL_DFRULE_WOSCARD, 2, PPDraftCreateRule::fUseGoodsLocAssoc);
		SetClusterData(CTL_DFRULE_WOSCARD, Data.Rec.Flags);

		SetupPPObjCombo(this, CTLSEL_DFRULE_SCARDSER, PPOBJ_SCARDSERIES, Data.Rec.SCardSerID, OLW_CANEDIT, 0);
	}
	setupCtrls();
	return 1;
}

int DraftCreateRuleDialog::getDTS(PPDfCreateRulePacket * pData)
{
	int    ok = 1;
	SString cash_nn;
    ushort v = 0;
	uint   sel_ctl = CTL_DFRULE_NAME;
	long   price_alg = 0, cost_alg = 0;
	StringSet ss(",");
	getCtrlData(CTL_DFRULE_NAME, Data.Rec.Name);
	THROW_PP(Data.Rec.Name[0] != '\0', PPERR_NAMENEEDED);
	if(!(Data.Rec.Flags & PPDraftCreateRule::fIsRulesGroup)) {
		long pay_type = 0;
		getCtrlData(CTLSEL_DFRULE_RULEGRP, &Data.Rec.ParentID);
		getCtrlString(sel_ctl = CTL_DFRULE_CASHNN, cash_nn);
		THROW_PP(Data.SetCashNN(cash_nn), PPERR_CASHNNNOTVALID);
		getCtrlData(CTLSEL_DFRULE_GGRP, &Data.Rec.GoodsGrpID);
		sel_ctl = CTL_DFRULE_OPKIND;
		getCtrlData(CTLSEL_DFRULE_OPKIND, &Data.Rec.OpID);
		THROW_PP(Data.Rec.OpID, PPERR_INVOP);
		getCtrlData(CTLSEL_DFRULE_OBJECT,  &Data.Rec.ArID);
		getCtrlData(CTLSEL_DFRULE_AGENT,  &Data.Rec.AgentID);
		getCtrlData(CTLSEL_DFRULE_PQUOT, &Data.Rec.PQuot);
		getCtrlData(CTLSEL_DFRULE_CQUOT, &Data.Rec.CQuot);
		getCtrlData(CTL_DFRULE_CPCTVAL, &Data.Rec.CPctVal);
		getCtrlData(CTL_DFRULE_PPCTVAL, &Data.Rec.PPctVal);
		GetClusterData(CTL_DFRULE_COSTALG,  &cost_alg);
		GetClusterData(CTL_DFRULE_PRICEALG, &price_alg);
		Data.Rec.PriceAlg = (int16)price_alg;
		Data.Rec.CostAlg  = (int16)cost_alg;
		if(Data.Rec.PQuot)
			Data.Rec.PriceAlg = PPDraftCreateRule::pByQuot;
		if(Data.Rec.CQuot)
			Data.Rec.CostAlg  = PPDraftCreateRule::cByQuot;
		if(Data.Rec.CostAlg != PPDraftCreateRule::cByPricePctVal)
			Data.Rec.CPctVal = 0;
		if(Data.Rec.PriceAlg != PPDraftCreateRule::pByCostPctVal)
			Data.Rec.PPctVal = 0;
		sel_ctl = CTL_DFRULE_CPCTVAL;
		THROW_PP(Data.Rec.CPctVal >= 0 && Data.Rec.CPctVal <= 100, PPERR_PERCENTINPUT);
		sel_ctl = CTL_DFRULE_PPCTVAL;
		THROW_PP(Data.Rec.PPctVal >= 0 && Data.Rec.PPctVal <= 100, PPERR_PERCENTINPUT);
		getCtrlData(CTL_DFRULE_EXCLGGRP, &v);
		SETFLAG(Data.Rec.Flags, PPDraftCreateRule::fExclGoodsGrp, v & 0x01);
		getCtrlData(CTL_DFRULE_MAXPOS, &Data.Rec.MaxPos);
		getCtrlData(CTL_DFRULE_MAXSUM, &Data.Rec.MaxSum);
		GetClusterData(CTL_DFRULE_PAYTYPE,  &pay_type);
		Data.Rec.Flags &= ~(PPDraftCreateRule::fOnlyBanking|PPDraftCreateRule::fOnlyNotBanking);
		Data.Rec.Flags |= pay_type;
		GetClusterData(CTL_DFRULE_WOSCARD,  &Data.Rec.Flags);
		getCtrlData(CTLSEL_DFRULE_SCARDSER, &Data.Rec.SCardSerID);
	}
	ASSIGN_PTR(pData, Data);
	CATCH
		ok = (selectCtrl(sel_ctl), 0);
	ENDCATCH
	return ok;
}
//
// PPObjDraftCreateRule
//
SLAPI PPDfCreateRulePacket::PPDfCreateRulePacket()
{
	Init();
}

int SLAPI PPDfCreateRulePacket::Init()
{
	MEMSZERO(Rec);
	CashNN.freeAll();
	return 1;
}

int SLAPI PPDfCreateRulePacket::CheckCash(PPID cash) const
{
	return CashNN.getCount() ? CashNN.bsearch(cash) : 1;
}

int SLAPI PPDfCreateRulePacket::GetCashNN(SString * pBuf, int delim) const
{
	PPID * p_id = 0;
	StringSet ss((char)delim, 0);
	for(uint i = 0; CashNN.enumItems(&i, (void**)&p_id) > 0;) {
		char buf[24];
		ltoa(*p_id, buf, 10);
		ss.add(buf, 0);
	}
	if(pBuf)
		*pBuf = ss.getBuf();
	return 1;
}

int SLAPI PPDfCreateRulePacket::GetCashNN(PPIDArray * pAry) const
{
	ASSIGN_PTR(pAry, CashNN);
	return 1;
}

int SLAPI PPDfCreateRulePacket::SetCashNN(const PPIDArray * pAry)
{
	if(pAry) {
		CashNN = *pAry;
		CashNN.sort();
	}
	else
		CashNN.freeAll();
	return 1;
}

int SLAPI PPDfCreateRulePacket::SetCashNN(const char * pBuf, int delim)
{
	int    ok = 1;
	if(pBuf) {
		SString cash_nn(pBuf);
		SString buf;
		PPIDArray ary;
		if(cash_nn.NotEmptyS()) {
			StringSet ss((char)delim, cash_nn);
			for(uint pos = 0; ss.get(&pos, buf) > 0;)
				if(buf.Strip().IsDigit())
					ary.addUnique(buf.ToLong());
				else {
					ok = 0;
					break;
				}
		}
		if(ok > 0)
			SetCashNN(&ary);
	}
	else
		ok = -1;
	return ok;
}

static int SLAPI RulesListFilt(void * rec, void * extraPtr)
{
	PPDraftCreateRule * p_rule = (PPDraftCreateRule*)rec;
	const long extra_param = (long)extraPtr;
	int  only_grps  = BIN(extra_param == PPDFCRRULE_ONLYGROUPS);
	int  only_rules = BIN(extra_param == PPDFCRRULE_ONLYRULES);
	int  is_rule = BIN(!(p_rule->Flags & PPDraftCreateRule::fIsRulesGroup));
	int  is_grp  = is_rule ? 0 : 1;
	return BIN(only_rules && is_rule || only_grps && is_grp || !extra_param || extra_param == p_rule->ParentID);
}

SLAPI PPObjDraftCreateRule::PPObjDraftCreateRule(void * extraPtr) : PPObjReference(PPOBJ_DFCREATERULE, extraPtr)
{
	filt = RulesListFilt;
}

int SLAPI PPObjDraftCreateRule::GetRules(PPID ruleGrpID, PPIDArray * pRules)
{
	PPIDArray rules;
	PPDraftCreateRule rule;
	MEMSZERO(rule);
	for(PPID id = 0; EnumItems(&id, &rule) > 0;)
		if(!ruleGrpID || rule.ParentID == ruleGrpID)
			rules.add(id);
	ASSIGN_PTR(pRules, rules);
	return 1;
}

// virtual
int SLAPI PPObjDraftCreateRule::Edit(PPID * pID, void * extraPtr)
{
	int    ok = cmCancel, r = cmCancel, valid_data = 0;
	ushort v = 0;
	int    is_new = (*pID == 0);
	DraftCreateRuleDialog * p_dlg = 0;
	TDialog * p_what_dlg = 0;
	PPDfCreateRulePacket pack;
	THROW(CheckRights(PPR_READ));
	if(!is_new) {
		THROW(GetPacket(*pID, &pack) > 0);
	}
	else {
		THROW(CheckDialogPtr(&(p_what_dlg = new TDialog(DLG_DFRULEADDW))));
		if((r = ExecView(p_what_dlg)) == cmOK) {
			p_what_dlg->getCtrlData(CTL_DFRULEADDW_WHAT, &(v = 0));
			SETFLAG(pack.Rec.Flags, PPDraftCreateRule::fIsRulesGroup, v & 0x01);
		}
	}
	if(!is_new || r == cmOK) {
		THROW(CheckDialogPtr(&(p_dlg = new DraftCreateRuleDialog)));
		p_dlg->setDTS(&pack);
		while(!valid_data && (ok = ExecView(p_dlg)) == cmOK) {
			if(p_dlg->getDTS(&pack) > 0) {
				THROW(CheckRightsModByID(pID));
				THROW(PutPacket(pID, &pack, 1));
				valid_data = 1;
			}
			else
				PPError();
		}
	}
	else
		ok = r;
	CATCHZOK
	delete p_what_dlg;
	delete p_dlg;
	return ok;
}

int SLAPI PPObjDraftCreateRule::GetPacket(PPID id, PPDfCreateRulePacket * pPack)
{
	int    ok = 1;
	PPDfCreateRulePacket rule;
	THROW((ok = PPRef->GetItem(PPOBJ_DFCREATERULE, id, &rule.Rec)));
	if(ok > 0) {
		SString cash_nn;
		THROW(PPRef->GetPropVlrString(PPOBJ_DFCREATERULE, id, DFCRRULPRP_CASHNN, cash_nn)); // AHTOXA
		THROW(rule.SetCashNN((const char*)cash_nn));
	}
	ASSIGN_PTR(pPack, rule);
	CATCHZOK
	return ok;
}

int SLAPI PPObjDraftCreateRule::PutPacket(PPID * pID, PPDfCreateRulePacket * pPack, int use_ta)
{
	int    ok = -1, del = 0;
	{
		PPTransaction tra(use_ta);
		THROW(tra);
		if(pPack) {
			pPack->Rec.Tag = PPOBJ_DFCREATERULE;
			pPack->Rec.ID  = 0;
		}
		if(pID && *pID) {
			if(pPack) {
				pPack->Rec.ID = *pID;
				THROW(ref->UpdateItem(PPOBJ_DFCREATERULE, *pID, &pPack->Rec, 1, 0));
			}
			else {
				THROW(ref->RemoveItem(PPOBJ_DFCREATERULE, *pID, 0));
				del = 1;
			}
		}
		else if(pPack) {
			THROW(ref->AddItem(PPOBJ_DFCREATERULE, pID, &pPack->Rec, 0));
		}
		if(pID && *pID) {
			SString cash_nn;
			if(!del)
				pPack->GetCashNN(&cash_nn);
			THROW(ref->PutPropVlrString(PPOBJ_DFCREATERULE, *pID, DFCRRULPRP_CASHNN, del ? 0 : cash_nn.cptr()));
		}
		THROW(tra.Commit());
	}
	ok = 1;
	CATCHZOK
	return ok;
}
// } AHTOXA

int SLAPI PPViewCSess::GetSessList(PPIDArray * pList)
{
	int    ok = 1;
	PPIDArray list;
	CSessViewItem item;
	for(InitIteration(ordByDefault); NextIteration(&item) > 0;)
		list.addUnique(item.ID);
	if(ok > 0) {
		list.sort();
		ASSIGN_PTR(pList, list);
	}
	return ok;
}

IMPL_CMPFUNC(CHKGOODSID, i1, i2)
{
	return cmp_long(((CCheckLineTbl::Rec*)i1)->GoodsID, ((CCheckLineTbl::Rec*)i2)->GoodsID);
}

struct _E {
	PPID   LocID;
	PPID   GoodsID;
	char   GoodsName[64];
	double Qtty;
	double Price;
};

#define COMPARE(a,b) ((a)>(b)) ? 1 : (((a)<(b)) ? -1 : 0)

IMPL_CMPFUNC(EGOODSLOC, i1, i2)
{
	_E * p_e1 = (_E*)i1;
	_E * p_e2 = (_E*)i2;
	int    r = COMPARE(p_e1->LocID, p_e2->LocID);
	return r ? r : stricmp866(p_e1->GoodsName, p_e2->GoodsName);
}

int SLAPI PPViewCSess::CreateDrafts(PPID ruleGrpID, PPID ruleID, PPID sessID)
{
	int    ok = 1;
	SString msg1, msg2, c_msg1, c_msg2;
	PPIDArray sess_list;
	PPIDArray rules;
	PPObjDraftCreateRule r_obj;
	PPObjGoods goods_obj; // ����� �� ����������� ������� ������ ����������
	PPWait(1);
	if(!sessID)
		GetSessList(&sess_list);
	else
		sess_list.add(sessID);
	if(!ruleID)
		r_obj.GetRules(ruleGrpID, &rules);
	if(!rules.getCount())
		rules.add(ruleID);
	PPLoadText(PPTXT_CRTGOODSLIST,   msg1);
	PPLoadText(PPTXT_ADDGOODSTOPACK, msg2);
	for(uint i = 0; i < sess_list.getCount(); i++)
		for(uint j = 0; j < rules.getCount(); j++) {
			c_msg1.Printf(msg1, i+1, sess_list.getCount(), j+1, rules.getCount());
			c_msg2.Printf(msg2, i+1, sess_list.getCount(), j+1, rules.getCount());
			//
			// �� ������ ���� {������-�������} ���������� ��������� ����������
			//
			THROW(ok = CreateDraft(rules.at(j), sess_list.at(i), c_msg1, c_msg2, 1));
		}
	CATCHZOKPPERR
	PPWait(0);
	return ok;
}

int SLAPI PPViewCSess::CreateDraft(PPID ruleID, PPID sessID, const SString & rMsg1, const SString & rMsg2, int use_ta)
{
	int    ok = -1;
	PPCashNode cn_rec;
	PPObjCashNode cn_obj;
	PPObjGoods g_obj;
	PPViewCCheck view;
	GoodsToObjAssoc * p_gds_assc = 0;
	if(ruleID && sessID) {
		_E _e, * p_e = 0;
		PPIDArray sess_list;
		PPDfCreateRulePacket rule;
		PPObjDraftCreateRule obj_rule;

		THROW(obj_rule.GetPacket(ruleID, &rule));
		THROW(CsObj.P_Tbl->GetSubSessList(sessID, &sess_list));
		if(rule.Rec.Flags & PPDraftCreateRule::fUseGoodsLocAssoc) {
			THROW_MEM(p_gds_assc = new GoodsToObjAssoc(PPASS_GOODS2LOC, PPOBJ_LOCATION));
			THROW(p_gds_assc->Load());
		}
		if(!sess_list.getCount() && sessID)
			sess_list.add(sessID);
		if(sess_list.getCount()) {
			LDATE  bill_dt = ZERODATE;
			LDATETIME last_check_dtm;
			SArray goods(sizeof(_E));
			PPIDArray cash_list;
			CCheckLineArray check_line_list;
			check_line_list.setDelta(64);
			CSessionTbl::Rec csess_rec;
			CCheckLineTbl::Rec * p_crec = 0;
			PPBillPacket b_pack;
			SString memo_buf, csess_buf;

			THROW(CsObj.Search(sessID, &csess_rec) > 0);
			bill_dt = csess_rec.Dt;
			rule.GetCashNN(&cash_list);
			//
			// ���������� ����� �� ��������� ���������
			//
			if((rule.Rec.Flags & PPDraftCreateRule::fWoSCard) || rule.Rec.SCardSerID || (rule.Rec.Flags & (PPDraftCreateRule::fOnlyBanking|PPDraftCreateRule::fOnlyNotBanking))) {
				long chk_count = 0;
				ObjIdListFilt chk_list_, sess_list_;
				PPIDArray chk_list;
				sess_list_.Set(&sess_list);
				THROW(CC.LoadChecksByList(&sess_list_, &cash_list, &chk_list_, &last_check_dtm));
				chk_list_.Sort();
				chk_list_.CopyTo(&chk_list);
				chk_count = (long)chk_list.getCount();
				for(long i = chk_count - 1; i >= 0; i--) {
					int to_del = 0;
					CCheckTbl::Rec chk_rec;
					if(CC.Search(chk_list.at((uint)i), &chk_rec) > 0) {
						if((rule.Rec.Flags & PPDraftCreateRule::fWoSCard) && chk_rec.SCardID)
							to_del = 1;
						else if(rule.Rec.SCardSerID) {
							SCardTbl::Rec scard_rec;
							MEMSZERO(scard_rec);
							CC.Cards.Search(chk_rec.SCardID, &scard_rec);
							if(chk_rec.SCardID == 0 || scard_rec.SeriesID != rule.Rec.SCardSerID)
								to_del = 1;
							to_del = (rule.Rec.Flags & PPDraftCreateRule::fExcludeSCardSer) ? !to_del : to_del;
						}
						if((rule.Rec.Flags & PPDraftCreateRule::fOnlyBanking) && !(chk_rec.Flags & CCHKF_BANKING))
							 to_del = 1;
						else if((rule.Rec.Flags & PPDraftCreateRule::fOnlyNotBanking) && (chk_rec.Flags & CCHKF_BANKING))
							 to_del = 1;
					}
					else
						to_del = 1;
					if(to_del)
						chk_list.atFree(i);
				}
				chk_list_.Set(&chk_list);
				THROW(CC.LoadLinesByList(0, &chk_list_, &check_line_list));
			}
			else {
				THROW(CC.LoadLinesBySessList(0, &sess_list, &cash_list, &check_line_list, &last_check_dtm));
			}
			PPID   def_loc_id = 0;
			if(cn_obj.Search(csess_rec.CashNodeID, &cn_rec) > 0)
				def_loc_id = cn_rec.LocID;
			for(uint p = 0; check_line_list.enumItems(&p, (void**)&p_crec) > 0;) {
				uint   pos = 0;
				const  double price = intmnytodbl(p_crec->Price) * p_crec->Quantity;
				const  double discount = p_crec->Dscnt * p_crec->Quantity;
				_e.GoodsID = p_crec->GoodsID;
				_e.Qtty    = p_crec->Quantity;
				_e.Price   = (rule.Rec.PriceAlg == PPDraftCreateRule::pByAvgSumDis) ? (price-discount) : price;
				_e.LocID   = 0;
				//
				// ���� ����������� ����� "������������ ���������� �����-�����", �� ����� ��������� ������ ��������� ��� ������� ������
				//
				if(p_gds_assc)
					p_gds_assc->Get(p_crec->GoodsID, &_e.LocID);
				_e.LocID = (_e.LocID == 0) ? def_loc_id : _e.LocID;
				if(goods.lsearch(&_e, &pos, PTR_CMPFUNC(_2long))) {
					p_e = (_E*)goods.at(pos);
					p_e->Qtty  += _e.Qtty;
					p_e->Price += _e.Price;
				}
				else {
					Goods2Tbl::Rec goods_rec;
					if(g_obj.Fetch(p_crec->GoodsID, &goods_rec) <= 0) {
						// @v9.7.0 if(g_obj.SearchByBarcode(CConfig.PrepayInvoiceGoodsCode, 0, &goods_rec) > 0)
						if(g_obj.Search(CConfig.PrepayInvoiceGoodsID, &goods_rec) > 0) // @v9.7.0
							p_crec->GoodsID = goods_rec.ID;
					}
					int    excl_ggrp = BIN(rule.Rec.Flags & PPDraftCreateRule::fExclGoodsGrp);
					int    grp_ok = BIN(g_obj.BelongToGroup(p_crec->GoodsID, rule.Rec.GoodsGrpID, 0));
					if(!rule.Rec.GoodsGrpID || ((grp_ok && !excl_ggrp) || (!grp_ok && excl_ggrp))) {
						//
						// ��-�� ����, ��� �� ������ ��� ����������, ������������� ������� ������ ��� ���
						//
						_e.GoodsID = p_crec->GoodsID;
						if(goods.lsearch(&_e, &(pos = 0), PTR_CMPFUNC(_2long))) {
							p_e = (_E*)goods.at(pos);
							p_e->Qtty  += _e.Qtty;
							p_e->Price += _e.Price;
						}
						else {
							STRNSCPY(_e.GoodsName, goods_rec.Name);
							THROW_SL(goods.insert(&_e));
						}
					}
				}
				if((p % 100) == 0)
					PPWaitPercent(p + 1, check_line_list.getCount(), rMsg1);
			}
			goods.sort(PTR_CMPFUNC(EGOODSLOC));

			int    cost_nominal = BIN(IsSellingOp(rule.Rec.OpID) <= 0);
			long   pos  = 0, prev_loc_id = (goods.getCount()) ? ((_E *)goods.at(0))->LocID : def_loc_id;
			double sum = 0.0, add_sum = 0.0;
			//
			//
			//
			{
				PPTransaction tra(use_ta);
				THROW(tra);
				THROW(b_pack.CreateBlank(rule.Rec.OpID, 0, prev_loc_id, 0));
				b_pack.Rec.LocID   = prev_loc_id;
				b_pack.Rec.Object  = rule.Rec.ArID;
				b_pack.Ext.AgentID = rule.Rec.AgentID;
				b_pack.Rec.Dt = bill_dt;
				STRNSCPY(b_pack.Ext.InvoiceCode, b_pack.Rec.Code);
				b_pack.Ext.InvoiceDate = bill_dt;
				PPObjCSession::MakeCodeString(&csess_rec, csess_buf);
				memo_buf.Z().CatChar('@').Cat("auto").CatDiv('-', 1).Cat(rule.Rec.Name).CatDiv('-', 1).Cat(csess_buf);
				memo_buf.CopyTo(b_pack.Rec.Memo, sizeof(b_pack.Rec.Memo));
				for(uint p = 0; goods.enumItems(&p, (void**)&p_e) > 0;) {
					int    new_doc_by_loc = 0;
					long   cost_alg  = rule.Rec.CostAlg;
					long   price_alg = rule.Rec.PriceAlg;
					PPID   loc_id = p_e->LocID;
					PPTransferItem ti;
					ReceiptTbl::Rec lot_rec;
					MEMSZERO(lot_rec);
					THROW(ti.Init(&b_pack.Rec));
					THROW(ti.SetupGoods(p_e->GoodsID) > 0);
					ti.Quantity_ = p_e->Qtty;
					new_doc_by_loc = BIN(rule.Rec.Flags & PPDraftCreateRule::fUseGoodsLocAssoc && prev_loc_id != loc_id);
					//
					// ����� ���� ����������� � ���������� �� ���������� ����
					//
					if(oneof2(price_alg, PPDraftCreateRule::pByLastLot, PPDraftCreateRule::pByQuot) ||
						oneof2(cost_alg, PPDraftCreateRule::cByLastLot, PPDraftCreateRule::cByQuot)) {
						int    r = 0;
						double price = 0.0, add_summ = 0.0;
						LDATE  oper_dt = LConfig.OperDate;
						//
						// LConfig.OperDate = last_check_dt ����� ��� ����, ����� ������� ��������� ��� �� �������� ����
						//
						DS.SetOperDate(last_check_dtm.d);
						//
						r = ::GetCurGoodsPrice(p_e->GoodsID, LConfig.Location, GPRET_MOSTRECENT | GPRET_OTHERLOC, &price, &lot_rec);
						//
						// LConfig.OperDate = oper_dt ���������� LConfig.OperDate � �������� ���������
						//
						DS.SetOperDate(oper_dt);
						THROW(r != GPRET_ERROR);
						//
						// ���� ���� ���������� 0, �� �� ����� ����� � �������� ���� ������� �������������� �� �����
						//
						if(lot_rec.Price == 0 && price_alg == PPDraftCreateRule::pByLastLot)
							price_alg = PPDraftCreateRule::pByAvgSum;
						//
						// ���� ���� ����������� 0, �� �� ����� ����� � �������� ���� - ���� ���������� - �������
						//
						if(lot_rec.Cost == 0 && cost_alg == PPDraftCreateRule::cByLastLot)
							cost_alg  = PPDraftCreateRule::cByPricePctVal;
					}
					//
					// ������������ ���� ����������� �� ���������
					//
					if(cost_alg == PPDraftCreateRule::cByQuot) {
						QuotIdent qi(loc_id, rule.Rec.CQuot, 0, 0);
						THROW(g_obj.GetQuotExt(p_e->GoodsID, qi, lot_rec.Cost, lot_rec.Price, &ti.Cost, 1));
						// ���� ���� �� ��������� 0, �� ����� ����� �� ���������� ����
						if(ti.Cost == 0.0)
							cost_alg = (lot_rec.Cost != 0.0) ? PPDraftCreateRule::cByLastLot : PPDraftCreateRule::cByPricePctVal;
					}
					//
					// ������������ ���� ���������� �� ���������
					//
					if(price_alg == PPDraftCreateRule::pByQuot) {
						QuotIdent qi(loc_id, rule.Rec.PQuot, 0, 0);
						THROW(g_obj.GetQuotExt(p_e->GoodsID, qi, lot_rec.Cost, lot_rec.Price, &ti.Price, 1));
						//
						// ���� ���� �� ��������� 0, �� ����� ����� � �������� ���� ������� �������������� �� ����
						//
						if(ti.Price == 0.0)
							price_alg = PPDraftCreateRule::pByAvgSum;
					}
					if(price_alg == PPDraftCreateRule::pByLastLot)
						ti.Price = lot_rec.Price;
					else if(price_alg != PPDraftCreateRule::pByQuot && price_alg != PPDraftCreateRule::pByCostPctVal)
						ti.Price = fdivnz(p_e->Price, p_e->Qtty);
					if(cost_alg == PPDraftCreateRule::cByLastLot)
						ti.Cost = lot_rec.Cost;
					else if(cost_alg != PPDraftCreateRule::cByQuot)
						ti.Cost = ti.Price - fdiv100r(ti.Price) * rule.Rec.CPctVal;
					if(price_alg == PPDraftCreateRule::pByCostPctVal) {
						ti.Cost  = (ti.Cost == 0.0) ? fdivnz(p_e->Price, p_e->Qtty) : ti.Cost;
						ti.Price = ti.Cost + fdiv100r(ti.Cost) * rule.Rec.PPctVal;
					}
					ti.Cost     = R5(ti.Cost);
					ti.Price    = R5(ti.Price);
					ti.Quantity_ = R6(ti.Quantity_);
					add_sum = ti.Quantity_ * (cost_nominal ? ti.Cost : ti.Price);
					if((rule.Rec.MaxPos && pos + 1 > rule.Rec.MaxPos) || (rule.Rec.MaxSum && sum + add_sum > rule.Rec.MaxSum) || new_doc_by_loc) {
						if((!rule.Rec.MaxSum || sum <= rule.Rec.MaxSum) && (!rule.Rec.MaxPos || pos <= rule.Rec.MaxPos) || new_doc_by_loc) {
							THROW(BillObj->__TurnPacket(&b_pack, 0, 1, 0));
							new_doc_by_loc = 0;
							ok = 1;
						}
						// @v9.5.3 (excess) b_pack.destroy();
						THROW(b_pack.CreateBlank(rule.Rec.OpID, 0, loc_id, 0));
						b_pack.Rec.LocID   = loc_id;
						b_pack.Rec.Object  = rule.Rec.ArID;
						b_pack.Ext.AgentID = rule.Rec.AgentID;
						b_pack.Rec.Dt = bill_dt;
						STRNSCPY(b_pack.Ext.InvoiceCode, b_pack.Rec.Code);
						b_pack.Ext.InvoiceDate = bill_dt;
						PPObjCSession::MakeCodeString(&csess_rec, csess_buf);
						memo_buf.Z().CatChar('@').Cat("auto").CatDiv('-', 1).Cat(rule.Rec.Name).CatDiv('-', 1).Cat(csess_buf);
						memo_buf.CopyTo(b_pack.Rec.Memo, sizeof(b_pack.Rec.Memo));
						pos = 0;
						sum = 0;
					}
					if((!rule.Rec.MaxPos || pos + 1 <= rule.Rec.MaxPos) && (!rule.Rec.MaxSum || sum + add_sum <= rule.Rec.MaxSum)) {
						ti.SetupSign(rule.Rec.OpID);
						THROW(b_pack.LoadTItem(&ti, 0, 0));
						sum += add_sum;
						pos++;
					}
					prev_loc_id = loc_id;
					PPWaitPercent(p, goods.getCount(), rMsg2);
				}
				if((!rule.Rec.MaxPos || pos <= rule.Rec.MaxPos) && (!rule.Rec.MaxSum || sum <= rule.Rec.MaxSum)) {
					THROW(BillObj->__TurnPacket(&b_pack, 0, 1, 0));
					ok = 1;
				}
				THROW(tra.Commit());
			}
		}
	}
	CATCHZOK
	ZDELETE(p_gds_assc);
	return ok;

}
// } AHTOXA

int SLAPI PPViewCSess::Detail(const void * pHdr, PPViewBrowser * pBrw)
{
	int    ok = -1;
	PPID   id = pHdr ? *(PPID *)pHdr : 0;
	if(id) {
		PPIDArray sub_sess_list;
		if(CsObj.P_Tbl->GetSubSessList(id, &sub_sess_list) > 0) {
			CSessFilt csflt;
			csflt.SuperSessID = id;
			SETFLAG(csflt.Flags, CSessFilt::fExtBill, Filt.Flags & CSessFilt::fExtBill);
			PPView::Execute(PPVIEW_CSESS, &csflt, PPView::exefModeless, 0);
		}
		else {
			CCheckFilt ccflt;
			ccflt.NodeList = Filt.NodeList_;
			ccflt.SessIDList.add(id);
			ViewCCheck(&ccflt, 0);
		}
	}
	return ok;
}

int SLAPI PPViewCSess::ViewExcesses(PPID id)
{
	int    ok = -1;
	uint   one_or_all = 0;
	long   fl = 0;
	if(id == 0)
		one_or_all = 1;
	if(one_or_all == 1 || SelectOneOrAll(&one_or_all, &fl) > 0) {
		CSessExcFilt cseflt;
		cseflt.CashNodeID = NodeList.GetSingle();
		cseflt.Flags = fl;
		if(one_or_all == 0)
			cseflt.SessIDList.add(id);
		else {
			CSessViewItem item;
			for(InitIteration(ordByDefault); NextIteration(&item) > 0;)
				cseflt.SessIDList.add(item.ID);
		}
		PPView::Execute(PPVIEW_CSESSEXC, &cseflt, PPView::exefModeless, 0);
	}
	return ok;
}

int SLAPI PPViewCSess::SelectOneOrAll(uint * pResult, long * pFlags)
{
	int    ok = -1;
	TDialog * dlg = new TDialog(pFlags ? DLG_SELCSESSEXC : DLG_SELCSESS);
	if(CheckDialogPtrErr(&dlg)) {
		ushort v = *pResult;
		dlg->setCtrlData(CTL_SELCSESS_SEL, &v);
		if(pFlags)
			dlg->setCtrlData(CTL_SELCSESS_FLAGS, &(v = 0));
		if(ExecView(dlg) == cmOK) {
			dlg->getCtrlData(CTL_SELCSESS_SEL, &(v = 0));
			*pResult = v;
			if(pFlags) {
				dlg->getCtrlData(CTL_SELCSESS_FLAGS, &(v = 0));
				SETFLAG(*pFlags, CSessExcFilt::fNoZeroAltGoods, v & 0x01);
			}
			ok = 1;
		}
	}
	else
		ok = 0;
	delete dlg;
	return ok;
}

int SLAPI PPViewCSess::GetBillList(PPID sessID, ObjIdListFilt & rList)
{
	int    ok = 1;
	rList.InitEmpty();
	PPIDArray bill_list;
	if(sessID) {
		THROW(BillObj->P_Tbl->GetPoolMembersList(PPASS_CSESSBILLPOOL, sessID, &bill_list));
		THROW(rList.Add(&bill_list));
	}
	else {
		CSessViewItem item;
		for(InitIteration(ordByDefault); NextIteration(&item) > 0;) {
			bill_list.clear();
			THROW(BillObj->P_Tbl->GetPoolMembersList(PPASS_CSESSBILLPOOL, item.ID, &bill_list));
			THROW(rList.Add(&bill_list));
		}
		BExtQuery::ZDelete(&P_IterQuery);
	}
	CATCHZOK
	return ok;
}

int SLAPI PPViewCSess::ViewGoodsTaxAnlz(PPID sessID)
{
	int    ok = -1;
	uint   one_or_all = 0;
	if(sessID == 0)
		one_or_all = 1;
	if(one_or_all == 1 || SelectOneOrAll(&one_or_all, 0) > 0) {
		GoodsTaxAnalyzeFilt flt;
		PPViewGoodsTaxAnalyze v_gdstax;
		THROW(GetBillList(one_or_all ? 0 : sessID, flt.BillList));
		if(v_gdstax.EditBaseFilt(&flt) > 0)
			ViewGoodsTaxAnalyze(&flt);
	}
	CATCHZOKPPERR
	return ok;
}

int SLAPI PPViewCSess::ViewAllChecks()
{
	int    ok = -1;
	CCheckFilt ccflt;
	CSessViewItem item;
	PPIDArray sub_sess_list;
	PPWait(1);
	for(InitIteration(ordByDefault); NextIteration(&item) > 0;) {
		sub_sess_list.clear();
		THROW(CsObj.P_Tbl->GetSubSessList(item.ID, &sub_sess_list));
		sub_sess_list.addUnique(item.ID);
		ccflt.SessIDList.addUnique(&sub_sess_list);
		PPWaitPercent(Counter);
	}
	if(ccflt.SessIDList.getCount())
		ViewCCheck(&ccflt, 0);
	PPWait(0);
	CATCHZOKPPERR
	return ok;
}

int SLAPI PPViewCSess::ViewGoodsOpAnlz(PPID sessID)
{
	int    ok = -1;
	uint   one_or_all = 0;
	if(sessID == 0)
		one_or_all = 1;
	if(one_or_all == 1 || SelectOneOrAll(&one_or_all, 0) > 0) {
		GoodsOpAnalyzeFilt flt;
		PPViewGoodsOpAnalyze v_gdsop;
		THROW(GetBillList(one_or_all ? 0 : sessID, flt.BillList));
		if(v_gdsop.EditBaseFilt(&flt) > 0)
			ViewGoodsOpAnalyze(&flt);
	}
	CATCHZOKPPERR
	return ok;
}

int SLAPI PPViewCSess::CloseSession()
{
	int    ok = -1;
	PPCashMachine * p_cm = 0;
	PPSyncCashSession  * p_scs = 0;
	PPAsyncCashSession * p_acs = 0;
	THROW_PP(Filt.NodeList_.GetSingle(), PPERR_UNDEFCASHNODE);
	THROW(p_cm = PPCashMachine::CreateInstance(Filt.NodeList_.GetSingle()));
	p_scs = p_cm->SyncInterface();
	if(p_scs) {
		ZDELETE(p_scs);
		THROW(p_cm->SyncCloseSession());
		ok = 1;
	}
	else {
		p_acs = p_cm->AsyncInterface();
		if(p_acs) {
			ZDELETE(p_acs);
			THROW(p_cm->AsyncCloseSession());
			ok = 1;
		}
	}
	CATCHZOKPPERR
	delete p_cm;
	return ok;
}

int SLAPI PPViewCSess::CompleteSession(PPID sessID)
{
	int    ok = -1;
	CSessionTbl::Rec sess_rec;
	PPCashMachine * p_cm = 0;
	PPSyncCashSession  * p_scs = 0;
	PPAsyncCashSession * p_acs = 0;
	if(CsObj.Search(sessID, &sess_rec) > 0 && sess_rec.CashNodeID) {
		PPObjCashNode cn_obj;
		PPCashNode cn_rec;
		PPObjSecur::Exclusion ose(PPEXCLRT_CSESSWROFF); // @v8.6.1
		THROW(cn_obj.Search(sess_rec.CashNodeID, &cn_rec) > 0);
		if(cn_rec.CashType == PPCMT_CASHNGROUP) {
			CSessGrouping csg;
			THROW(csg.CompleteSession(sessID));
			ok = 1;
		}
		else {
			THROW(p_cm = PPCashMachine::CreateInstance(sess_rec.CashNodeID));
			p_scs = p_cm->SyncInterface();
			if(p_scs) {
				THROW(ok = p_scs->CompleteSession(sessID));
			}
			else {
				p_acs = p_cm->AsyncInterface();
				if(p_acs)
					THROW(ok = p_acs->CompleteSession(sessID));
			}
		}
	}
	CATCHZOKPPERR
	delete p_scs;
	delete p_acs;
	delete p_cm;
	return ok;
}

int SLAPI PPViewCSess::RecalcSession(PPID sessID)
{
	int    ok = -1;
	uint   v = 0;
	int    r = SelectorDialog(DLG_RCVRCSESS, CTL_RCVRCSESS_WHAT, &v);
	if(r > 0) {
		if(oneof3(v, 0, 1, 2)) {
			if(sessID && CsObj.Search(sessID) > 0) {
				if(v == 1)
					v = 5;
				else if(v == 2)
					v = 10;
				ok = CsObj.ReWriteOff(sessID, v, 1) ? 1 : PPErrorZ();
			}
		}
		else if(v == 3) {
			PPIDArray sess_list;
			PPViewCSess::GetSessList(&sess_list);
			CsObj.Recover(sess_list);
		}
		else if(v == 4) {
			PPIDArray sess_list;
			PPViewCSess::GetSessList(&sess_list);
			if(sess_list.getCount()) {
				ObjTransmitParam param;
				if(ObjTransmDialog(DLG_OBJTRANSM, &param) > 0) {
					const PPIDArray & rary = param.DestDBDivList.Get();
					PPObjIDArray objid_ary;
					PPWait(1);
					objid_ary.Add(PPOBJ_CSESSION, sess_list);
					param.Flags |= ObjTransmitParam::fRecoverTransmission;
					THROW(PPObjectTransmit::Transmit(&rary, &objid_ary, &param));
					ok = 1;
				}
			}
		}
	}
	CATCHZOKPPERR
	return ok;
}

int SLAPI PPViewCSess::DetachSessFromSuperSess(PPID sessID)
{
	int    ok = -1;
	CSessionTbl::Rec sess_rec;
	THROW(CsObj.CheckRights(CSESSRT_CORRECT));
	if(sessID && Filt.SuperSessID) {
		PPTransaction tra(1);
		THROW(tra);
		if(CsObj.Search(sessID, &sess_rec) > 0 && sess_rec.SuperSessID == Filt.SuperSessID && sess_rec.WrOffAmount > 0.0) {
			CsObj.P_Tbl->data.SuperSessID = 0;
			THROW_DB(CsObj.P_Tbl->updateRec());
			THROW(CsObj.Recalc(Filt.SuperSessID, 0));
			ok = 1;
		}
		THROW(tra.Commit());
	}
	CATCHZOKPPERR
	return ok;
}

int SLAPI PPViewCSess::Transmit(PPID id, int transmitKind)
{
	int    ok = -1;
	if(transmitKind == 1) {
		/* @v9.1.3
		PPImpExpParam csess_param, ccheck_param, ccline_param;
		THROW(LoadSdRecord(PPREC_CSESS,   &csess_param.InrRec));
		THROW(LoadSdRecord(PPREC_CCHECKS, &ccheck_param.InrRec));
		THROW(LoadSdRecord(PPREC_CCLINES, &ccline_param.InrRec));
		if(SelCSessImpExpParams(&csess_param, &ccheck_param, &ccline_param, 0) > 0) {
			PPWait(1);
			PPCSessExporter cses_exp;
			PPIDArray sess_list; // @vmiller
			THROW(cses_exp.Init(&csess_param, &ccheck_param, &ccline_param));
			GetSessList(&sess_list); // @vmiller
			//THROW(cses_exp.PutSess(id, 0)); // @vmiller comment
			THROW(cses_exp.PutSess(&sess_list, 0)); // @vmiller
		}
		*/
		PPIDArray sess_list;
		sess_list.add(id);
		PPPosProtocol pp;
		THROW(pp.ExportPosSession(sess_list, 0, 0, 0));
	}
	else {
		ObjTransmitParam param;
		if(id) 
			param.Flags |= param.fQueryInmassTransmission;
		if(id && ObjTransmDialog(DLG_OBJTRANSM, &param) > 0) {
			CSessViewItem item;
			const PPIDArray & rary = param.DestDBDivList.Get();
			PPObjIDArray objid_ary;
			PPWait(1);
			if(id && !(param.Flags & param.fInmassTransmission))
				objid_ary.Add(PPOBJ_CSESSION, id);
			else {
				for(InitIteration(ordByDefault); NextIteration(&item) > 0;)
					objid_ary.Add(PPOBJ_CSESSION, item.ID);
			}
			THROW(PPObjectTransmit::Transmit(&rary, &objid_ary, &param));
			ok = 1;
		}
	}
	CATCHZOKPPERR
	PPWait(0);
	return ok;
}

int SLAPI PPViewCSess::AddItem()
{
	PPID   id = 0;
	return (CsObj.Edit(&id, 0) == cmOK) ? 1 : -1;
}

int SLAPI PPViewCSess::EditItem(PPID id)
{
	return (id > 0 && CsObj.Edit(&id, 0) == cmOK) ? 1 : -1;
}

int SLAPI PPViewCSess::DeleteItem(PPID sessID)
{
	int    ok = -1;
	TDialog * dlg = 0;
	CSessionTbl::Rec sess_rec;
	if(CsObj.Search(sessID, &sess_rec) > 0 && sess_rec.CashNodeID && !sess_rec.SuperSessID) {
		PPObjCashNode cn_obj;
		PPCashNode cn_rec;
		THROW(CsObj.CheckRights(PPR_DEL));
		THROW(cn_obj.Search(sess_rec.CashNodeID, &cn_rec) > 0);
		if(cn_rec.CashType == PPCMT_CASHNGROUP || PPCashMachine::IsSyncCMT(cn_rec.CashType) || PPCashMachine::IsAsyncCMT(cn_rec.CashType)) {
			int    grade = -1;
			THROW(CheckDialogPtr(&(dlg = new TDialog(DLG_CSESSDEL))));
			dlg->setCtrlUInt16(CTL_CSESSDEL_OPTIONS, 0);
			if(ExecView(dlg) == cmOK) {
				int    is_confirmed = 1;
				ushort v = dlg->getCtrlUInt16(CTL_CSESSDEL_OPTIONS);
				if(v == 0)
					grade = CSESSINCMPL_GLINES;
				else if(v == 1)
					grade = CSESSINCMPL_CHECKS;
				else if(v == 2)
					grade = CSESSINCMPL_COMPLETE;
				if(PPCashMachine::IsSyncCMT(cn_rec.CashType) && grade == CSESSINCMPL_COMPLETE) {
					THROW_PP(cn_rec.CurSessID != sessID, PPERR_RMVACTIVECSESS);
					if(!CONFIRMCRIT(PPCFM_DELSYNCCSESS))
						is_confirmed = 0;
				}
				if(is_confirmed) {
					CSessGrouping csg;
					THROW(csg.RemoveSession(sessID, grade));
					ok = 1;
					PPWait(1);
				}
				PPWait(0);
			}
		}
	}
	CATCHZOKPPERR
	delete dlg;
	return ok;
}

int SLAPI PPViewCSess::CalcTotal(CSessTotal * pTotal)
{
	int    ok = 1;
	CSessViewItem item;
	memzero(pTotal, sizeof(CSessTotal));
	for(InitIteration(ordByDefault); NextIteration(&item) > 0;) {
		pTotal->SessCount++;
		pTotal->CheckCount     += item.ChkCount;
		pTotal->Amount         += item.Amount;
		pTotal->Discount       += item.Discount;
		pTotal->WrOffAmount    += item.WrOffAmount;
		pTotal->AggrRest       += item.AggrRest;
		pTotal->WrOffCost      += item.WrOffCost;
		pTotal->Income         += item.Income;
		pTotal->BnkAmount      += item.BnkAmount;    // @CSCardAmount
		pTotal->CSCardAmount   += item.CSCardAmount; // @v6.2.1
		pTotal->WORetAmount    += item.WORetAmount;
		pTotal->WORetBnkAmount += item.WORetBnkAmount;
		pTotal->BnkDiscount    += item.BnkDiscount;
	}
	return ok;
}

int SLAPI PPViewCSess::ViewTotal()
{
	int    ok = -1;
	TDialog * dlg = 0;
	CSessTotal total;
	CalcTotal(&total);
	if(CheckDialogPtrErr(&(dlg = new TDialog((Filt.Flags & CSessFilt::fExtBill) ? DLG_CSESSWRETTOTAL : DLG_CSESSTOTAL)))) {
		double  ret_amt = total.WORetAmount - total.Amount;
		SetPeriodInput(dlg, CTL_CSESSTOTAL_PERIOD, &Filt.Period);
		dlg->setCtrlData(CTL_CSESSTOTAL_COUNT,    &total.SessCount);
		dlg->setCtrlData(CTL_CSESSTOTAL_CHKCOUNT, &total.CheckCount);
		dlg->setCtrlData(CTL_CSESSTOTAL_AMTWORET, &total.WORetAmount);
		dlg->setCtrlData(CTL_CSESSTOTAL_AMTRET,   &ret_amt);
		dlg->setCtrlData(CTL_CSESSTOTAL_AMOUNT,   &total.Amount);
		if(Filt.Flags & CSessFilt::fExtBill) {
			dlg->setCtrlData(CTL_CSESSTOTAL_INCOME, &total.Income);
			dlg->setCtrlData(CTL_CSESSTOTAL_WROFFRCPT, &total.WrOffCost);
		}
		else
			dlg->disableCtrls(1, CTL_CSESSTOTAL_INCOME, CTL_CSESSTOTAL_WROFFRCPT, 0);
		dlg->setCtrlData(CTL_CSESSTOTAL_WROFF,     &total.WrOffAmount);
		dlg->setCtrlData(CTL_CSESSTOTAL_DISCOUNT,  &total.Discount);
		dlg->setCtrlData(CTL_CSESSTOTAL_EXC,       &total.AggrRest);
		dlg->setCtrlData(CTL_CSESSTOTAL_BANKING,   &total.BnkAmount);
		ExecViewAndDestroy(dlg);
	}
	else
		ok = 0;
	return ok;
}

int SLAPI PPViewCSess::PosPrint(PPID curID)
{
	int   ok = -1;
	PPID  parent_node_id = 0;
	PPCashMachine * p_cm = 0;
	CSessInfo  cs_info;
	if(CsObj.Search(curID, &cs_info.Rec) > 0 && cs_info.Rec.CashNodeID) {
		if(PPObjCashNode::IsExtCashNode(cs_info.Rec.CashNodeID, &parent_node_id) && parent_node_id)
			THROW(PPObjCashNode::Lock(parent_node_id));
		THROW(p_cm = PPCashMachine::CreateInstance(cs_info.Rec.CashNodeID));
		CC.GetSessTotal(curID, CsObj.P_Tbl->GetCcGroupingFlags(cs_info.Rec, 0) | CCheckCore::gglfUseFullCcPackets, &cs_info.Total, 0);
		THROW(p_cm->SyncPrintZReportCopy(&cs_info));
	}
	CATCHZOKPPERR
	if(parent_node_id)
		PPObjCashNode::Unlock(parent_node_id);
	delete p_cm;
	return ok;
}

int SLAPI PPViewCSess::Print(const void * pHdr)
{
	return Helper_Print((Filt.Flags & CSessFilt::fExtBill) ? REPORT_CSESSVIEWEXT : REPORT_CSESSVIEW, 0);
}

int SLAPI PPViewCSess::PrintSession(PPID sessID)
{
	CSessionTbl::Rec ses_rec;
	if(CsObj.Search(sessID, &ses_rec) > 0) {
		PView  pv(sessID);
		PPAlddPrint(REPORT_CSESS_KM6, &pv);
	}
	return -1;
}
//
// @ModuleDef(PPViewCSessExc)
//
IMPLEMENT_PPFILT_FACTORY(CSessExc); SLAPI CSessExcFilt::CSessExcFilt() : PPBaseFilt(PPFILT_CSESSEXC, 0, 0)
{
	SetFlatChunk(offsetof(CSessExcFilt, ReserveStart),
		offsetof(CSessExcFilt, SessIDList)-offsetof(CSessExcFilt, ReserveStart));
	SetBranchSArray(offsetof(CSessExcFilt, SessIDList));
	Init(1, 0);
}

SLAPI PPViewCSessExc::PPViewCSessExc() : PPView(0, &Filt, PPVIEW_CSESSEXC)
{
	P_TempTbl = 0;
}

SLAPI PPViewCSessExc::~PPViewCSessExc()
{
	delete P_TempTbl;
}

PPBaseFilt * SLAPI PPViewCSessExc::CreateFilt(void * extraPtr) const
{
	PPBaseFilt * p_base_filt = 0;
	if(PPView::CreateFiltInstance(PPFILT_CSESSEXC, &p_base_filt)) {
		PPObjCashNode cn_obj;
		((CSessExcFilt *)p_base_filt)->CashNodeID = cn_obj.GetSingle();
	}
	return p_base_filt;
}

int SLAPI PPViewCSessExc::EditBaseFilt(PPBaseFilt * pFilt)
{
#define GRP_GOODS 1
	int    ok = -1;
	TDialog * dlg = 0;
	THROW(Filt.IsA(pFilt));
	THROW(CheckDialogPtr(&(dlg = new TDialog(DLG_CSESSEXC))));
	{
		CSessExcFilt * p_filt = (CSessExcFilt *)pFilt;
		PPID   cashnode_id = p_filt->CashNodeID;
		GoodsCtrlGroup::Rec rec(p_filt->GoodsGrpID, p_filt->GoodsID, 0, GoodsCtrlGroup::enableSelUpLevel);
		dlg->addGroup(GRP_GOODS, new GoodsCtrlGroup(CTLSEL_CSESSEXC_GGRP, CTLSEL_CSESSEXC_GOODS));
		dlg->SetupCalPeriod(CTLCAL_CSESSEXC_PERIOD, CTL_CSESSEXC_PERIOD);
		SetPeriodInput(dlg, CTL_CSESSEXC_PERIOD, &p_filt->Period);
		dlg->setGroupData(GRP_GOODS, &rec);
		SETIFZ(cashnode_id, CsObj.GetEqCfg().DefCashNodeID);
		SetupPPObjCombo(dlg, CTLSEL_CSESSEXC_NODE, PPOBJ_CASHNODE, cashnode_id, 0, 0);
		dlg->AddClusterAssoc(CTL_CSESSEXC_FLAGS, 0, CSessExcFilt::fNoZeroAltGoods);
		dlg->SetClusterData(CTL_CSESSEXC_FLAGS, p_filt->Flags);
		for(int valid_data = 0; !valid_data && ExecView(dlg) == cmOK;) {
			GetPeriodInput(dlg, CTL_CSESSEXC_PERIOD, &p_filt->Period);
			dlg->getCtrlData(CTLSEL_CSESSEXC_NODE, &p_filt->CashNodeID);
			dlg->getGroupData(GRP_GOODS, &rec);
			p_filt->GoodsGrpID = rec.GrpID;
			p_filt->GoodsID    = rec.GoodsID;
			dlg->GetClusterData(CTL_CSESSEXC_FLAGS, &p_filt->Flags);
			ok = valid_data = 1;
		}
	}
	CATCHZOKPPERR
	delete dlg;
	return ok;
#undef GRP_GOODS
}

int SLAPI PPViewCSessExc::GatherGoodsLines(int sign, PPID goodsID, CSessDfctList * pList)
{
	int    ok = -1;
	if(!Filt.GoodsID || goodsID == Filt.GoodsID) {
		if(Filt.SessIDList.getCount()) {
			for(uint i = 0; i < Filt.SessIDList.getCount(); i++)
				THROW(Tbl.GetDfctList(sign, Filt.SessIDList.at(i), goodsID, &Filt.Period, 0, pList, 0));
		}
		else
			THROW(Tbl.GetDfctList(sign, 0, goodsID, &Filt.Period, 0, pList, 0));
		if(pList->getCount())
			ok = 1;
	}
	CATCH
		ok = 0;
		pList->freeAll();
	ENDCATCH
	return ok;
}

PP_CREATE_TEMP_FILE_PROC(CreateTempFile, TempCSessExc);

int SLAPI PPViewCSessExc::SetRecValues(TempCSessExcTbl::Rec & trec, const CGoodsLineTbl::Rec * pRec, int add)
{
	if(add) {
		trec.Qtty += pRec->Qtty;
		trec.Rest += pRec->Rest;
		trec.Sum  += pRec->Sum;
		trec.AltGoodsQtty += pRec->AltGoodsQtty;
	}
	else {
		trec.Qtty = pRec->Qtty;
		trec.Rest = pRec->Rest;
		trec.Sum  = pRec->Sum;
		trec.AltGoodsQtty = pRec->AltGoodsQtty;
		GetAltGoodsPrice(trec.AltGoodsID, &trec.AltGoodsPrice);
	}
	trec.Price   = trec.Sum / trec.Qtty;
	trec.RestSum = trec.Rest * trec.Price;
	trec.AltGoodsSum = trec.AltGoodsQtty * trec.AltGoodsPrice;
	trec.AltDiff = trec.AltGoodsSum-trec.Price*trec.AltGoodsQtty;
	return 1;
}

int SLAPI PPViewCSessExc::AddItemToTempTable(const PPID orgGoodsID, CGoodsLineTbl::Rec * pRec, LAssocArray * pRgAssoc)
{
	int    ok  = 1;
	uint   pos = 0;
	PPID   goods_id = pRec->GoodsID;
	//
	// ���� ���� ������� ��� ������ goods_id, �� �������� �������������
	// goods_id �� ��������������� � ���.
	//
	pRgAssoc->Search(goods_id, &goods_id, &pos);
	TempCSessExcTbl::Rec trec;
	TempCSessExcTbl::Key0 tk;
	tk.Sign = pRec->Sign;
	tk.GoodsID = goods_id;
	if(P_TempTbl->search(0, &tk, spEq)) {
		int    to_update = 1;
		P_TempTbl->copyBufTo(&trec);
		if(trec.AltGoodsID != pRec->AltGoodsID) {
			if(Filt.Flags & CSessExcFilt::fNoZeroAltGoods) {
				while(P_TempTbl->search(0, &tk, spNext) && tk.GoodsID == goods_id) {
					if(P_TempTbl->data.AltGoodsID == pRec->AltGoodsID) {
						P_TempTbl->copyBufTo(&trec);
						to_update = 2;
						break;
					}
				}
				if(to_update == 1) {
					trec.OrgGoodsID = orgGoodsID;
					trec.AltGoodsID = pRec->AltGoodsID;
					SetRecValues(trec, pRec, 0);
					THROW_DB(P_TempTbl->insertRecBuf(&trec));
					to_update = 0;
				}
			}
			else
				trec.AltGoodsID = -1L;
		}
		if(to_update) {
			SetRecValues(trec, pRec, 1);
			THROW_DB(P_TempTbl->updateRecBuf(&trec));
		}
	}
	else {
		SysJournal * p_sj = DS.GetTLA().P_SysJ;
		PPID   key_goods_id = goods_id;
		MEMSZERO(trec);
		int    found = 0;
		do {
			Goods2Tbl::Rec goods_rec;
			if(GObj.Fetch(goods_id, &goods_rec) > 0) {
				goods_id = goods_rec.ID;
				if(goods_id != key_goods_id) {
					pRgAssoc->Add(key_goods_id, goods_id, 0);
					pRec->GoodsID = goods_id;
					THROW(AddItemToTempTable(orgGoodsID, pRec, pRgAssoc)); // Recursion
				}
				else {
					trec.Sign = pRec->Sign;
					trec.GoodsID  = goods_id;
					STRNSCPY(trec.GoodsName, goods_rec.Name);
					trec.UnitID   = goods_rec.UnitID;
					trec.OrgGoodsID = orgGoodsID;
					trec.AltGoodsID = pRec->AltGoodsID;
					SetRecValues(trec, pRec, 0);
					THROW_DB(P_TempTbl->insertRecBuf(&trec));
				}
				found = 1;
			}
		} while(!found && p_sj && p_sj->GetLastObjUnifyEvent(PPOBJ_GOODS, goods_id, &goods_id, 0) > 0);
		if(!found) {
			trec.Sign = pRec->Sign;
			trec.GoodsID    = goods_id;
			ideqvalstr(goods_id, trec.GoodsName, sizeof(trec.GoodsName));
			trec.OrgGoodsID = orgGoodsID;
			trec.AltGoodsID = pRec->AltGoodsID;
			SetRecValues(trec, pRec, 0);
			THROW_DB(P_TempTbl->insertRecBuf(&trec));
		}
	}
	CATCHZOK
	return ok;
}

int SLAPI PPViewCSessExc::MakeTempTable(PPID sessID)
{
	int    ok = 1;
	IterCounter counter;
	LAssocArray rg_assoc;
	PPObjUnit  uobj;
	BExtQuery  * q   = 0;
	DBQ        * dbq = 0;
	CGoodsLineTbl::Key0 k, k_;
	q = new BExtQuery(&Tbl, 0, 64);
	dbq = ppcheckfiltid(dbq, Tbl.SessID, sessID);
	dbq = & (*dbq && daterange(Tbl.Dt, &Filt.Period));
	dbq = ppcheckfiltid(dbq, Tbl.GoodsID, Filt.GoodsID);
	if(Filt.Flags & CSessExcFilt::fNoZeroAltGoods)
		dbq = & (*dbq && Tbl.AltGoodsID > 0L);
	q->selectAll().where(*dbq);

	MEMSZERO(k);
	k.SessID   = sessID;
	k.Dt       = Filt.Period.low;
	k.GoodsID  = Filt.GoodsID;
	counter.Init(q->countIterations(0, &(k_ = k), spGe));
	for(q->initIteration(0, &k, spGe); q->nextIteration() > 0;) {
		PPWaitPercent(counter.Increment());
		if(Filt.CashNodeID) {
			CSessionTbl::Rec csess_rec;
			if(CsObj.Search(Tbl.data.SessID, &csess_rec) <= 0 || csess_rec.CashNodeID != Filt.CashNodeID)
				continue;
		}
		if(!(Filt.Flags & CSessExcFilt::fNoZeroAltGoods) && R6(Tbl.data.Rest) == 0)
			continue;
		if(Tbl.data.Qtty != 0 && (!Filt.GoodsGrpID || Filt.GoodsID || GObj.BelongToGroup(Tbl.data.GoodsID, Filt.GoodsGrpID, 0))) {
			CGoodsLineTbl::Rec rec;
			Tbl.copyBufTo(&rec);
			THROW(AddItemToTempTable(rec.GoodsID, &rec, &rg_assoc));
		}
	}
	CATCHZOK
	delete q;
	return ok;
}

int SLAPI PPViewCSessExc::_MakeTempTable(int clearBefore)
{
	int    ok = 1;
	if(P_TempTbl) {
		if(clearBefore) {
			THROW_DB(deleteFrom(P_TempTbl, 0, *(DBQ *)0));
		}
		if(Filt.SessIDList.getCount()) {
			for(uint i = 0; i < Filt.SessIDList.getCount(); i++)
				THROW(MakeTempTable(Filt.SessIDList.at(i)));
		}
		else
			THROW(MakeTempTable(0L));
	}
	else
		ok = -1;
	CATCHZOK
	return ok;
}

PPID SLAPI PPViewCSessExc::GetCommonLoc()
{
	if(CommonLocID < 0) {
		CommonLocID = 0;
		PPCashNode cn_rec;
		if(Filt.CashNodeID) {
			if(SearchObject(PPOBJ_CASHNODE, Filt.CashNodeID, &cn_rec) > 0)
				CommonLocID = cn_rec.LocID;
		}
		else
			for(uint i = 0; i < Filt.SessIDList.getCount(); i++) {
				CSessionTbl::Rec csess_rec;
				if(CsObj.Search(Filt.SessIDList.at(i), &csess_rec) > 0)
					if(SearchObject(PPOBJ_CASHNODE, csess_rec.CashNodeID, &cn_rec) > 0)
						if(cn_rec.LocID)
							if(!CommonLocID)
								CommonLocID = cn_rec.LocID;
							else if(CommonLocID != cn_rec.LocID) {
								CommonLocID = 0;
								break;
							}
			}
	}
	return CommonLocID;
}

int SLAPI PPViewCSessExc::Init_(const PPBaseFilt * pFilt)
{
	int    ok = 1;
	if(Helper_InitBaseFilt(pFilt)) {
		ZDELETE(P_TempTbl);
		CommonLocID = -1;
		if((P_TempTbl = CreateTempFile()) == 0 || !_MakeTempTable(0)) {
			ok = 0;
			ZDELETE(P_TempTbl);
		}
	}
	else
		ok = 0;
	return ok;
}

// AHTOXA {
int SLAPI PPViewCSessExc::GetAltGoodsPrice(PPID goodsID, double * pPrice)
{
	double price = 0.0;
	if(goodsID) {
		LDATE  dt = ZERODATE;
		PPID   last_sess_id = Filt.SessIDList.getLast();
		if(last_sess_id) {
			CSessionTbl::Rec csess_rec;
			if(CsObj.Search(last_sess_id, &csess_rec) > 0)
				dt = csess_rec.Dt;
		}
		dt = NZOR(dt, LConfig.OperDate);
		ReceiptTbl::Rec lot_rec;
		if(BillObj->trfr->Rcpt.GetLastLot(goodsID, GetCommonLoc(), dt, &lot_rec) > 0)
			price = R5(lot_rec.Price);
	}
	ASSIGN_PTR(pPrice, price);
	return 1;
}
// } AHTOXA

int SLAPI PPViewCSessExc::InitIteration()
{
	int    ok = 1;
	char   k[MAXKEYLEN];
	BExtQuery::ZDelete(&P_IterQuery);
	THROW_PP(P_TempTbl, PPERR_PPVIEWNOTINITED);
	THROW_MEM(P_IterQuery = new BExtQuery(P_TempTbl, 0, 16));
	P_IterQuery->selectAll();
	memzero(k, sizeof(k));
	P_IterQuery->initIteration(0, k, spFirst);
	CATCHZOK
	return ok;
}

int FASTCALL PPViewCSessExc::NextIteration(CSessExcViewItem * pItem)
{
	int    ok = -1;
	if(P_IterQuery && P_IterQuery->nextIteration() > 0) {
		ASSIGN_PTR(pItem, P_TempTbl->data);
		ok = 1;
	}
	return ok;
}

DBQuery * SLAPI PPViewCSessExc::CreateBrowserQuery(uint * pBrwId, SString * pSubTitle)
{
	DBE    dbe_goods;
	TempCSessExcTbl * t = new TempCSessExcTbl(P_TempTbl->fileName);
	PPDbqFuncPool::InitObjNameFunc(dbe_goods, PPDbqFuncPool::IdObjNameGoods, t->AltGoodsID);
	DBQuery * q = & (select(
		t->GoodsID,       // #0
		t->Sign,          // #1
		t->GoodsName,     // #2
		t->Qtty,          // #3
		t->Rest,          // #4
		t->Price,         // #5
		t->RestSum,       // #6
		dbe_goods,        // #7
		t->AltGoodsQtty,  // #8
		t->AltGoodsPrice, // #9
		t->AltGoodsSum,   // #10
		t->AltDiff,       // #11
		0L).from(t, 0L)).orderBy(t->Sign, t->GoodsName, 0L);
	if(!CheckQueryPtr(q))
		ZDELETE(q);
	ASSIGN_PTR(pBrwId, BROWSER_CSESSEXC);
	return q;
}

int SLAPI PPViewCSessExc::ProcessCommand(uint ppvCmd, const void * pHdr, PPViewBrowser * pBrw)
{
	struct H {
		PPID   GoodsID;
		int16  Sign;
	} * p_hdr = (H *)pHdr;
	int    ok = PPView::ProcessCommand(ppvCmd, pHdr, pBrw);
	if(ok == -2) {
		PPID   goods_id = p_hdr ? p_hdr->GoodsID : 0;
		switch(ppvCmd) {
			case PPVCMD_EDITGOODS:
				ok = -1;
				if(goods_id && GObj.Edit(&goods_id, 0) == cmOK)
					ok = 1;
				break;
			case PPVCMD_VIEWLOTS:
				ok = -1;
				if(goods_id)
					::ViewLots(goods_id, 0, 0, 0, 0);
				break;
			case PPVCMD_SETALTGOODS:
				ok = -1;
				if(p_hdr && SetAltGoods(p_hdr->Sign, p_hdr->GoodsID) > 0)
					ok = 1;
				break;
			case PPVCMD_PUTTOBASKETALL:
				ok = -1;
				ConvertDeficitToBasket();
				break;
		}
	}
	return ok;
}

struct CSessExcAltGoodsParam {
	PPID   GoodsID;
	PPID   AltGoodsID;
	PPID   LocID;
	int    SelExistsGoodsOnly;
	double Price;
};

#define GRP_GOODS 1

class CSessExcAltGoodsDialog : public TDialog {
public:
	CSessExcAltGoodsDialog(const PPEquipConfig & rCfg) : TDialog(DLG_CSEXCAG)
	{
		addGroup(GRP_GOODS, new GoodsCtrlGroup(CTLSEL_CSEXCAG_GGRP, CTLSEL_CSEXCAG_GOODS));
		disableCtrls(1, CTL_CSEXCAG_DFCTDT, CTL_CSEXCAG_DFCTCOST, CTL_CSEXCAG_DFCTPRICE,
			CTL_CSEXCAG_ALGDT, CTL_CSEXCAG_ALGCOST, CTL_CSEXCAG_ALGPRICE, CTL_CSEXCAG_SELLPRICE, 0);
		(SubstPriceRange = rCfg.DeficitSubstPriceDevRange).Scale(0.001);
	}
	int    setDTS(const CSessExcAltGoodsParam *);
	int    getDTS(PPID * pAltGoodsID);
private:
	DECL_HANDLE_EVENT;
	int    SetupLastLotCtrs(PPID goodsID, int isDfctGoods);
	CSessExcAltGoodsParam Data;
	RealRange SubstPriceRange;
};

IMPL_HANDLE_EVENT(CSessExcAltGoodsDialog)
{
	TDialog::handleEvent(event);
	if(event.isCmd(cmLot)) {
		GoodsCtrlGroup::Rec grp_rec;
		getGroupData(GRP_GOODS, &grp_rec);
		if(grp_rec.GoodsID)
			ViewLots(grp_rec.GoodsID, 0 /* LocID */, 0, 0, 0);
	}
	else if(event.isCmd(cmCBSelected)) {
 		GoodsCtrlGroup::Rec grp_rec;
		getGroupData(GRP_GOODS, &grp_rec);
		SetupLastLotCtrs(grp_rec.GoodsID, 0);
	}
	else if(event.isClusterClk(CTL_CSEXCAG_FLAGS)) {
		GoodsCtrlGroup * p_grp = (GoodsCtrlGroup *)getGroup(GRP_GOODS);
		if(p_grp)
			p_grp->setFlagExistsOnly(this, getCtrlUInt16(CTL_CSEXCAG_FLAGS));
	}
	else
		return;
	clearEvent(event);
}

int CSessExcAltGoodsDialog::SetupLastLotCtrs(PPID goodsID, int isDfctGoods)
{
	LDATE  dt = ZERODATE;
	double cost = 0.0, price = 0.0;
	if(goodsID) {
		ReceiptTbl::Rec lot;
		MEMSZERO(lot);
		BillObj->trfr->Rcpt.GetCurrentGoodsPrice(goodsID, 0, GPRET_INDEF, &price, &lot);
		cost = lot.Cost;
		dt   = lot.Dt;
	}
	if(isDfctGoods) {
		setCtrlData(CTL_CSEXCAG_DFCTDT,    &dt);
		setCtrlReal(CTL_CSEXCAG_DFCTCOST,  cost);
		setCtrlReal(CTL_CSEXCAG_DFCTPRICE, price);
	}
	else {
		enableCommand(cmLot, BIN(goodsID));
		setCtrlData(CTL_CSEXCAG_ALGDT,    &dt);
		setCtrlReal(CTL_CSEXCAG_ALGCOST,  cost);
		setCtrlReal(CTL_CSEXCAG_ALGPRICE, price);
	}
	return 1;
}

int CSessExcAltGoodsDialog::setDTS(const CSessExcAltGoodsParam * pData)
{
	Data = *pData;
	PPObjGoods gobj;
	Goods2Tbl::Rec goods_rec;
	PPID   grp_id = 0;
	if(gobj.Fetch(Data.GoodsID, &goods_rec) > 0) {
		grp_id = goods_rec.ParentID;
		setStaticText(CTL_CSEXCAG_ST_MAINGOODS, goods_rec.Name);
	}
	setCtrlUInt16(CTL_CSEXCAG_FLAGS, BIN(Data.SelExistsGoodsOnly));
	GoodsCtrlGroup::Rec grp_rec(Data.AltGoodsID ? 0L : grp_id, Data.AltGoodsID, Data.LocID,
		Data.SelExistsGoodsOnly ? GoodsCtrlGroup::existsGoodsOnly : 0);
	setGroupData(GRP_GOODS, &grp_rec);
	SetupLastLotCtrs(Data.GoodsID, 1);
	setCtrlReal(CTL_CSEXCAG_SELLPRICE, Data.Price);
	SetupLastLotCtrs(Data.AltGoodsID, 0);
	return 1;
}

int CSessExcAltGoodsDialog::getDTS(PPID * pAltGoodsID)
{
	int    ok = 1;
	GoodsCtrlGroup::Rec grp_rec;
	getGroupData(GRP_GOODS, &grp_rec);
	if(!SubstPriceRange.IsZero()) {
		double new_price = 0.0;
		SString msg_buf;
		getCtrlData(CTL_CSEXCAG_ALGPRICE, &new_price);
		msg_buf.Cat(SubstPriceRange.low*100.0, MKSFMTD(0, 1, NMBF_NOTRAILZ|NMBF_NOZERO)).
			Dot().Dot().Cat(SubstPriceRange.upp*100.0, MKSFMTD(0, 1, NMBF_NOTRAILZ|NMBF_NOZERO));
		double rel_diff = fdivnz(new_price - Data.Price, Data.Price);
		THROW_PP_S(SubstPriceRange.CheckVal(rel_diff), PPERR_INVCOSTRANGE, msg_buf);
	}
	ASSIGN_PTR(pAltGoodsID, grp_rec.GoodsID);
	CATCHZOK
	return ok;
}

int SLAPI PPViewCSessExc::SetAltGoods(int sign, PPID goodsID)
{
	int    ok = -1, r;
	uint   i;
	PPID   alt_goods_id = 0;
	PPID   save_loc_id = LConfig.Location;
	CSessExcAltGoodsDialog * dlg = 0;
	CSessDfctItem * p_item;
	CSessDfctList cgl_list;
	TempCSessExcTbl::Rec rec;
	TempCSessExcTbl::Key0 k0;
	k0.Sign = sign;
	k0.GoodsID = goodsID;
	if(SearchByKey(P_TempTbl, 0, &k0, &rec) > 0) {
		PPID   org_goods_id = rec.OrgGoodsID;
		THROW(r = GatherGoodsLines(sign, org_goods_id, &cgl_list));
		if(r > 0) {
			for(i = 0; alt_goods_id >= 0 && cgl_list.enumItems(&i, (void**)&p_item);)
				if(p_item->AltGoodsID != alt_goods_id)
					alt_goods_id = alt_goods_id ? -1L : p_item->AltGoodsID;
			if(alt_goods_id < 0)
				PPMessage(mfInfo|mfOK, PPINF_AMBIGUITYCSEXCALTGOODS);
			else {
				CSessExcAltGoodsParam param;
				param.GoodsID    = goodsID;
				param.AltGoodsID = alt_goods_id;
				param.SelExistsGoodsOnly = param.AltGoodsID ? 0 : 1;
				param.LocID = GetCommonLoc();
				param.Price = rec.Price;
				if(param.LocID)
					DS.SetLocation(param.LocID);
				THROW(CheckDialogPtr(&(dlg = new CSessExcAltGoodsDialog(CsObj.GetEqCfg()))));
				dlg->setDTS(&param);
				for(int valid_data = 0; !valid_data && ExecView(dlg) == cmOK;) {
					if(dlg->getDTS(&alt_goods_id) > 0) {
						PPTransaction tra(1);
						THROW(tra);
						for(i = 0; cgl_list.enumItems(&i, (void**)&p_item);) {
							THROW_DB(updateFor(&Tbl, 0, (Tbl.SessID == p_item->SessID && Tbl.Dt == p_item->Dt &&
								Tbl.GoodsID == p_item->GoodsID && Tbl.Sign == (long)sign),
								set(Tbl.AltGoodsID, dbconst(alt_goods_id))));
   						}
						THROW(_MakeTempTable(1));
						THROW(tra.Commit());
						ok = valid_data = 1;
					}
					else
						PPError();
				}
			}
		}
	}
	CATCHZOKPPERR
	delete dlg;
	DS.SetLocation(save_loc_id);
	return ok;
}

int SLAPI PPViewCSessExc::ConvertDeficitToBasket()
{
	int    ok = -1, r = 0;
	SelBasketParam param;
	param.SelPrice = 1;
	THROW(r = GetBasketByDialog(&param, GetSymb()));
	if(r > 0) {
		CSessExcViewItem item;
		PPWait(1);
		for(InitIteration(); NextIteration(&item) > 0;) {
			ILTI   i_i;
			ReceiptTbl::Rec lot_rec;
			MEMSZERO(lot_rec);
			THROW(::GetCurGoodsPrice(item.GoodsID, CommonLocID, GPRET_MOSTRECENT, 0, &lot_rec) != GPRET_ERROR);
			i_i.GoodsID     = item.GoodsID;
			i_i.UnitPerPack = lot_rec.UnitPerPack;
			i_i.Cost        = R5(lot_rec.Cost);
			if(param.SelPrice == 1)
				i_i.Price = R5(lot_rec.Cost);
			else if(param.SelPrice == 2)
				i_i.Price = R5(lot_rec.Price);
			else if(param.SelPrice == 3)
				i_i.Price = item.Price;
			else
				i_i.Price = item.Price;
			i_i.Price = (i_i.Price == 0.0) ? item.Price : i_i.Price;
			i_i.CurPrice = 0.0;
			i_i.Flags    = 0;
			i_i.Suppl    = lot_rec.SupplID;
			i_i.QCert    = lot_rec.QCertID;
			i_i.Expiry   = lot_rec.Expiry;
			i_i.Quantity = fabs(item.Rest);
			THROW(param.Pack.AddItem(&i_i, 0, param.SelReplace));
		}
		PPWait(0);
		THROW(GoodsBasketDialog(param, 1));
		ok = 1;
	}
	CATCHZOKPPERR
	PPWait(0);
	return ok;
}

int SLAPI PPViewCSessExc::ViewTotal()
{
	int    ok = 1;
	long   count = 0;
	double total_rest    = 0.0;
	double total_sumrest = 0.0;
	double total_altsum  = 0.0;
	double total_altdiff = 0.0;
	CSessExcViewItem item;
	TDialog * dlg = 0;
	if(CheckDialogPtrErr(&(dlg = new TDialog(DLG_CSESSEXCTOTAL)))) {
		for(InitIteration(); NextIteration(&item) > 0;) {
			count++;
			if(item.Sign > 0) {
				total_rest += item.Rest;
				if(item.Rest != item.Qtty)
					total_sumrest += fdivnz(item.Sum * item.Rest, item.Qtty);
				else
					total_sumrest += item.Sum;
				total_altsum  += item.AltGoodsSum;
				total_altdiff += item.AltDiff;
			}
			else {
				total_rest -= item.Rest;
				if(item.Rest != item.Qtty)
					total_sumrest -= fdivnz(item.Sum * item.Rest, item.Qtty);
				else
					total_sumrest -= item.Sum;
				total_altsum  -= item.AltGoodsSum;
				total_altdiff -= item.AltDiff;
			}
		}
		SetPeriodInput(dlg, CTL_CSESSEXCTOTAL_PERIOD, &Filt.Period);
		dlg->setCtrlLong(CTL_CSESSEXCTOTAL_COUNT, count);
		dlg->setCtrlReal(CTL_CSESSEXCTOTAL_QTTY,  total_rest);
		dlg->setCtrlReal(CTL_CSESSEXCTOTAL_REST,  total_sumrest);
		dlg->setCtrlReal(CTL_CSESSEXCTOTAL_ALTSUM, total_altsum);
		dlg->setCtrlReal(CTL_CSESSEXCTOTAL_ALTDIF, total_altdiff);
		ExecViewAndDestroy(dlg);
	}
	else
		ok = 0;
	return ok;
}

int SLAPI PPViewCSessExc::Print(const void *)
{
	uint   rpt_id = (Filt.Flags & CSessExcFilt::fNoZeroAltGoods) ? REPORT_CSESSEXCALT : REPORT_CSESSEXC;
	return Helper_Print(rpt_id);
}
//
// Implementation of PPALDD_CSession
//
PPALDD_CONSTRUCTOR(CSession)
{
	if(Valid) {
		AssignHeadData(&H, sizeof(H));
		Extra[0].Ptr = new PPObjCSession;
	}
}

PPALDD_DESTRUCTOR(CSession)
{
	Destroy();
	delete (PPObjCSession*)Extra[0].Ptr;
}

int PPALDD_CSession::InitData(PPFilt & rFilt, long rsrv)
{
	int    ok = -1;
	if(rFilt.ID == H.ID)
		ok = DlRtm::InitData(rFilt, rsrv);
	else {
		MEMSZERO(H);
		H.ID = rFilt.ID;
		CSessionTbl::Rec rec;
		if(((PPObjCSession*)Extra[0].Ptr)->Search(rFilt.ID, &rec) > 0) {
			H.ID          = rec.ID;
			H.CashNodeID  = rec.CashNodeID;
			H.SuperSessID = rec.SuperSessID;
			H.SessNumber  = rec.SessNumber;
			H.CashNumber  = rec.CashNumber;
			H.Dt          = rec.Dt;
			H.Tm          = rec.Tm;
			H.Amount      = rec.Amount;
			H.Discount    = rec.Discount;
			H.AggrAmount  = rec.AggrAmount;
			H.AggrRest    = rec.AggrRest;
			H.WrOffAmount = rec.WrOffAmount;
			H.Banking     = rec.BnkAmount;
			H.CSCardAmount = rec.CSCardAmount; // @v8.8.5
			ok = DlRtm::InitData(rFilt, rsrv);
		}
	}
	return ok;
}
//
// Implementation of PPALDD_CSessionView
//
PPALDD_CONSTRUCTOR(CSessionView)
{
	if(Valid) {
		AssignHeadData(&H, sizeof(H));
		AssignIterData(1, &I, sizeof(I));
	}
}

PPALDD_DESTRUCTOR(CSessionView)
{
	Destroy();
}

int PPALDD_CSessionView::InitData(PPFilt & rFilt, long rsrv)
{
	INIT_PPVIEW_ALDD_DATA_U(CSess, rsrv);
	H.FltSuperSessID = p_filt->SuperSessID;
	H.FltCashNodeID  = p_filt->NodeList_.GetSingle();
	H.FltCashNumber  = p_filt->CashNumber;
	H.FltBeg  = p_filt->Period.low;
	H.FltEnd  = p_filt->Period.upp;
	return DlRtm::InitData(rFilt, rsrv);
}

int PPALDD_CSessionView::InitIteration(PPIterID iterId, int sortId, long /*rsrv*/)
{
	INIT_PPVIEW_ALDD_ITER_ORD(CSess, sortId);
}

int PPALDD_CSessionView::NextIteration(PPIterID iterId)
{
	START_PPVIEW_ALDD_ITER(CSess);
	I.SessID          = item.ID;
	I.SessIncomplete  = item.Incomplete;
	I.WrOffRcptAmount = item.WrOffCost;
	I.Income          = item.Income;
	I.CheckCount      = item.ChkCount;
	I.WORetAmount     = item.WORetAmount;
	I.RetAmount       = item.WORetAmount - item.Amount;
	FINISH_PPVIEW_ALDD_ITER();
}

void PPALDD_CSessionView::Destroy()
{
	DESTROY_PPVIEW_ALDD(CSess);
}
//
// Implementation of PPALDD_CSessExc
//
PPALDD_CONSTRUCTOR(CSessExc)
{
	if(Valid) {
		AssignHeadData(&H, sizeof(H));
		AssignIterData(1, &I, sizeof(I));
	}
}

PPALDD_DESTRUCTOR(CSessExc)
{
	Destroy();
}

int PPALDD_CSessExc::InitData(PPFilt & rFilt, long rsrv)
{
	INIT_PPVIEW_ALDD_DATA_U(CSessExc, rsrv);
	CSessionCore cs_core;
	CSessionTbl::Rec cs_rec;
	int    num_sess = p_filt->SessIDList.getCount();
	H.FltSessID     = num_sess ? p_filt->SessIDList.at(0) : 0L;
	H.FltCashNodeID = p_filt->CashNodeID;
	H.FltGoodsGrpID = p_filt->GoodsGrpID;
	H.FltGoodsID    = p_filt->GoodsID;
	H.FltBeg        = p_filt->Period.low;
	H.FltEnd        = p_filt->Period.upp;
	H.FltFlags      = p_filt->Flags;
	H.fNoZeroAltGoods = BIN(p_filt->Flags & CSessExcFilt::fNoZeroAltGoods);
	H.FltSessNum   = num_sess;
	H.FltSessEndDt = ZERODATE;
	H.FltSessEndTm = ZEROTIME;
	if(num_sess && cs_core.Search(p_filt->SessIDList.at(0), &cs_rec) > 0) {
		H.FltSessBegDt = cs_rec.Dt;
		H.FltSessBegTm = cs_rec.Tm;
		H.FltSessIncompl = cs_rec.Incomplete;
		if(--num_sess && cs_core.Search(p_filt->SessIDList.at(num_sess), &cs_rec) > 0) {
			H.FltSessEndDt = cs_rec.Dt;
			H.FltSessEndTm = cs_rec.Tm;
		}
	}
	else {
		H.FltSessBegDt = ZERODATE;
		H.FltSessBegTm = ZEROTIME;
		H.FltSessIncompl = 0;
	}
	return DlRtm::InitData(rFilt, rsrv);
}

int PPALDD_CSessExc::InitIteration(PPIterID iterId, int sortId, long /*rsrv*/)
{
	INIT_PPVIEW_ALDD_ITER(CSessExc);
}

int PPALDD_CSessExc::NextIteration(PPIterID iterId)
{
	START_PPVIEW_ALDD_ITER(CSessExc);
	I.GoodsID = item.GoodsID;
	I.AltGoodsID = item.AltGoodsID;
	I.Qtty    = item.Qtty;
	I.Rest    = item.Rest;
	I.Price   = item.Price;
	I.Sum     = item.Sum;
	I.RestSum = item.RestSum;
	I.AltGoodsPrice = item.AltGoodsPrice;
	I.AltGoodsQtty  = item.AltGoodsQtty;
	I.AltGoodsSum   = item.AltGoodsSum;
	I.AltDiff       = item.AltDiff;
	FINISH_PPVIEW_ALDD_ITER();
}

void PPALDD_CSessExc::Destroy()
{
	DESTROY_PPVIEW_ALDD(CSessExc);
}
