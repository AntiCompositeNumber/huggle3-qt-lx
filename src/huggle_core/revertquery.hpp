//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef REVERTQUERY_H
#define REVERTQUERY_H

#include "definitions.hpp"

#include "apiquery.hpp"
#include "collectable_smartptr.hpp"
#include "editquery.hpp"
#include "mediawikiobject.hpp"
#include "wikiedit.hpp"
#include <QString>
#include <QDateTime>

class QTimer;

namespace Huggle
{
    class ApiQuery;
    class QueryResult;
    class EditQuery;
    class WikiEdit;
    class WikiSite;

    /*!
     * \brief The RevertQuery class can be used to rollback any edit
     */
    class HUGGLE_EX_CORE RevertQuery : public QObject, public Query, public MediaWikiObject
    {
            Q_OBJECT
        public:
            static QString GetCustomRevertStatus(QueryResult *result_data, WikiSite *site, bool *failed, bool *suspend);

            RevertQuery(WikiSite *site);
            RevertQuery(WikiEdit *edit, WikiSite *site);
            ~RevertQuery() override;
            void Process() override;
            void Restart() override;
            //! In case you want to revert only last edit, set this to true
            void SetLast();
            void Kill() override;
            QString QueryTargetToString() override;
            bool IsProcessed() override;
            void SetUsingSR(bool software_rollback);
            bool IsUsingSR();
            WikiSite *GetSite() override;
            WikiEdit *GetEdit();
            //! Time when a query was issued (this is set externaly)
            QDateTime Date;
            QString Summary = "";
            //! Rollback with no check if it's a good idea or not (revert even whitelisted users, sysops etc)
            bool IgnorePreflightCheck = false;
            bool MinorEdit = true;
        public slots:
            void OnTick();
        private:
            void displayError(QString error, QString reason = "");
            QString getCustomRevertStatus(bool *failed);
            void preflightCheck();
            void evaluatePreflightCheck();
            bool evaluateRevertQueryResults();
            void cancelRevert();
            bool evaluateRevert();
            void executeRollback();
            void executeRevert();
            void freeResources();
            //! Whether software rollback should be used instead of regular rollback
            bool usingSR = false;
            Collectable_SmartPtr<ApiQuery> qPreflight;
            Collectable_SmartPtr<ApiQuery> qRevert;
            Collectable_SmartPtr<ApiQuery> qHistoryInfo;
            Collectable_SmartPtr<ApiQuery> qRetrieve;
            Collectable_SmartPtr<EditQuery> eqSoftwareRollback;
            Collectable_SmartPtr<WikiEdit> editToBeReverted;
            QTimer *timer = nullptr;
            //! Revert only and only last edit
            bool oneEditOnly = false;
            bool rollingBack = false;
            bool preflightFinished = false;
            int SR_RevID;
            int SR_Depth;
            QString SR_Target = "";
    };

    inline WikiSite *RevertQuery::GetSite()
    {
        if (this->Site == nullptr)
            return (this->editToBeReverted->GetSite());

        // we know the site and despite it may be inconsistent we return it because that is what
        // programmer wanted (by inconsistent I mean the revert query could have different site
        // than the edit has)
        return this->Site;
    }
}

#endif // REVERTQUERY_H
