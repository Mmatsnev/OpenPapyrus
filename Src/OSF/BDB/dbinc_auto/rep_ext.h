/* DO NOT EDIT: automatically built by dist/s_include. */
#ifndef	_rep_ext_h_
#define	_rep_ext_h_

#if defined(__cplusplus)
extern "C" {
#endif

int __rep_bulk_marshal __P((ENV *, __rep_bulk_args *, uint8 *, size_t, size_t *));
int __rep_bulk_unmarshal __P((ENV *, __rep_bulk_args *, uint8 *, size_t, uint8 **));
int __rep_control_marshal __P((ENV *, __rep_control_args *, uint8 *, size_t, size_t *));
int __rep_control_unmarshal __P((ENV *, __rep_control_args *, uint8 *, size_t, uint8 **));
int __rep_egen_marshal __P((ENV *, __rep_egen_args *, uint8 *, size_t, size_t *));
int __rep_egen_unmarshal __P((ENV *, __rep_egen_args *, uint8 *, size_t, uint8 **));
int __rep_fileinfo_marshal __P((ENV *, uint32, __rep_fileinfo_args *, uint8 *, size_t, size_t *));
int __rep_fileinfo_unmarshal __P((ENV *, uint32, __rep_fileinfo_args **, uint8 *, size_t, uint8 **));
int __rep_grant_info_marshal __P((ENV *, __rep_grant_info_args *, uint8 *, size_t, size_t *));
int __rep_grant_info_unmarshal __P((ENV *, __rep_grant_info_args *, uint8 *, size_t, uint8 **));
int __rep_logreq_marshal __P((ENV *, __rep_logreq_args *, uint8 *, size_t, size_t *));
int __rep_logreq_unmarshal __P((ENV *, __rep_logreq_args *, uint8 *, size_t, uint8 **));
int __rep_newfile_marshal __P((ENV *, __rep_newfile_args *, uint8 *, size_t, size_t *));
int __rep_newfile_unmarshal __P((ENV *, __rep_newfile_args *, uint8 *, size_t, uint8 **));
int __rep_update_marshal __P((ENV *, uint32, __rep_update_args *, uint8 *, size_t, size_t *));
int __rep_update_unmarshal __P((ENV *, uint32, __rep_update_args **, uint8 *, size_t, uint8 **));
int __rep_vote_info_marshal __P((ENV *, __rep_vote_info_args *, uint8 *, size_t, size_t *));
int __rep_vote_info_unmarshal __P((ENV *, __rep_vote_info_args *, uint8 *, size_t, uint8 **));
int __rep_vote_info_v5_marshal __P((ENV *, __rep_vote_info_v5_args *, uint8 *, size_t, size_t *));
int __rep_vote_info_v5_unmarshal __P((ENV *, __rep_vote_info_v5_args *, uint8 *, size_t, uint8 **));
void __rep_lsn_hist_key_marshal __P((ENV *, __rep_lsn_hist_key_args *, uint8 *));
int __rep_lsn_hist_key_unmarshal __P((ENV *, __rep_lsn_hist_key_args *, uint8 *, size_t, uint8 **));
void __rep_lsn_hist_data_marshal __P((ENV *, __rep_lsn_hist_data_args *, uint8 *));
int __rep_lsn_hist_data_unmarshal __P((ENV *, __rep_lsn_hist_data_args *, uint8 *, size_t, uint8 **));
int __rep_update_req __P((ENV *, __rep_control_args *));
int __rep_page_req __P((ENV *, DB_THREAD_INFO *, int, __rep_control_args *, DBT *));
int __rep_update_setup __P((ENV *, int, __rep_control_args *, DBT *, __time64_t, DB_LSN *));
int __rep_bulk_page __P((ENV *, DB_THREAD_INFO *, int, __rep_control_args *, DBT *));
int __rep_page __P((ENV *, DB_THREAD_INFO *, int, __rep_control_args *, DBT *));
int __rep_init_cleanup __P((ENV *, REP *, int));
int __rep_pggap_req __P((ENV *, REP *, __rep_fileinfo_args *, uint32));
int __rep_finfo_alloc __P((ENV *, __rep_fileinfo_args *, __rep_fileinfo_args **));
int __rep_remove_init_file(ENV *);
int __rep_reset_init(ENV *);
int __rep_elect_pp __P((DB_ENV *, uint32, uint32, uint32));
int __rep_elect_int __P((ENV *, uint32, uint32, uint32));
int __rep_vote1 __P((ENV *, __rep_control_args *, DBT *, int));
int __rep_vote2 __P((ENV *, __rep_control_args *, DBT *, int));
int __rep_update_grant __P((ENV *, db_timespec *));
int __rep_islease_granted(ENV *);
int __rep_lease_table_alloc __P((ENV *, uint32));
int __rep_lease_grant __P((ENV *, __rep_control_args *, DBT *, int));
int __rep_lease_check __P((ENV *, int));
int __rep_lease_refresh(ENV *);
int __rep_lease_expire(ENV *);
db_timeout_t __rep_lease_waittime(ENV *);
int __rep_allreq __P((ENV *, __rep_control_args *, int));
int __rep_log __P((ENV *, DB_THREAD_INFO *, __rep_control_args *, DBT *, int, __time64_t, DB_LSN *));
int __rep_bulk_log __P((ENV *, DB_THREAD_INFO *, __rep_control_args *, DBT *, __time64_t, DB_LSN *));
int __rep_logreq __P((ENV *, __rep_control_args *, DBT *, int));
int __rep_loggap_req __P((ENV *, REP *, DB_LSN *, uint32));
int __rep_logready __P((ENV *, REP *, __time64_t, DB_LSN *));
int __rep_env_create(DB_ENV *);
void __rep_env_destroy(DB_ENV *);
int __rep_get_config __P((DB_ENV *, uint32, int *));
int __rep_set_config __P((DB_ENV *, uint32, int));
int __rep_start_pp __P((DB_ENV *, DBT *, uint32));
int __rep_start_int __P((ENV *, DBT *, uint32));
int __rep_open_sysdb __P((ENV *, DB_THREAD_INFO *, DB_TXN *, const char *, uint32, DB **));
int __rep_client_dbinit __P((ENV *, int, repdb_t));
int __rep_get_limit __P((DB_ENV *, uint32 *, uint32 *));
int __rep_set_limit __P((DB_ENV *, uint32, uint32));
int __rep_set_nsites_pp(DB_ENV *, uint32);
int __rep_set_nsites_int __P((ENV *, uint32));
int __rep_get_nsites(DB_ENV *, uint32 *);
int __rep_set_priority(DB_ENV *, uint32);
int __rep_get_priority(DB_ENV *, uint32 *);
int __rep_set_timeout __P((DB_ENV *, int, db_timeout_t));
int __rep_get_timeout __P((DB_ENV *, int, db_timeout_t *));
int __rep_get_request __P((DB_ENV *, db_timeout_t *, db_timeout_t *));
int __rep_set_request __P((DB_ENV *, db_timeout_t, db_timeout_t));
int __rep_set_transport_pp __P((DB_ENV *, int, int (*)(DB_ENV *, const DBT *, const DBT *, const DB_LSN *, int, uint32)));
int __rep_set_transport_int __P((ENV *, int, int (*)(DB_ENV *, const DBT *, const DBT *, const DB_LSN *, int, uint32)));
int __rep_get_clockskew __P((DB_ENV *, uint32 *, uint32 *));
int __rep_set_clockskew __P((DB_ENV *, uint32, uint32));
int __rep_flush(DB_ENV *);
int __rep_sync(DB_ENV *, uint32);
int __rep_txn_applied __P((ENV *, DB_THREAD_INFO *, DB_COMMIT_INFO *, db_timeout_t));
int __rep_process_message_pp __P((DB_ENV *, DBT *, DBT *, int, DB_LSN *));
int __rep_process_message_int __P((ENV *, DBT *, DBT *, int, DB_LSN *));
int __rep_apply __P((ENV *, DB_THREAD_INFO *, __rep_control_args *, DBT *, DB_LSN *, int *, DB_LSN *));
int __rep_process_txn(ENV *, DBT *);
int __rep_resend_req __P((ENV *, int));
int __rep_check_doreq __P((ENV *, REP *));
int __rep_check_missing __P((ENV *, uint32, DB_LSN *));
int __rep_open(ENV *);
int __rep_close_diagfiles(ENV *);
int __rep_env_refresh(ENV *);
int __rep_env_close(ENV *);
int __rep_preclose(ENV *);
int __rep_closefiles(ENV *);
int __rep_write_egen __P((ENV *, REP *, uint32));
int __rep_write_gen __P((ENV *, REP *, uint32));
int __rep_stat_pp __P((DB_ENV *, DB_REP_STAT **, uint32));
int __rep_stat_print_pp(DB_ENV *, uint32);
int __rep_stat_print __P((ENV *, uint32));
int __rep_bulk_message __P((ENV *, REP_BULK *, REP_THROTTLE *, DB_LSN *, const DBT *, uint32));
int __rep_send_bulk __P((ENV *, REP_BULK *, uint32));
int __rep_bulk_alloc __P((ENV *, REP_BULK *, int, uintptr_t *, uint32 *, uint32));
int __rep_bulk_free __P((ENV *, REP_BULK *, uint32));
int __rep_send_message __P((ENV *, int, uint32, DB_LSN *, const DBT *, uint32, uint32));
int __rep_new_master __P((ENV *, __rep_control_args *, int));
void __rep_elect_done __P((ENV *, REP *));
int __env_rep_enter __P((ENV *, int));
int __env_db_rep_exit(ENV *);
int __db_rep_enter __P((DB *, int, int, int));
int __op_handle_enter(ENV *);
int __op_rep_enter __P((ENV *, int, int));
int __op_rep_exit(ENV *);
int __archive_rep_enter(ENV *);
int __archive_rep_exit(ENV *);
int __rep_lockout_archive __P((ENV *, REP *));
int __rep_lockout_api __P((ENV *, REP *));
int __rep_take_apilockout(ENV *);
int __rep_clear_apilockout(ENV *);
int __rep_lockout_apply __P((ENV *, REP *, uint32));
int __rep_lockout_msg __P((ENV *, REP *, uint32));
int __rep_send_throttle __P((ENV *, int, REP_THROTTLE *, uint32, uint32));
uint32 __rep_msg_to_old __P((uint32, uint32));
uint32 __rep_msg_from_old __P((uint32, uint32));
int __rep_print_system __P((ENV *, uint32, const char *, ...)) __attribute__ ((__format__ (__printf__, 3, 4)));
int __rep_print __P((ENV *, uint32, const char *, ...)) __attribute__ ((__format__ (__printf__, 3, 4)));
void __rep_print_message __P((ENV *, int, __rep_control_args *, char *, uint32));
void __rep_fire_event __P((ENV *, uint32, void *));
void __rep_msg __P((const ENV *, const char *));
int __rep_notify_threads __P((ENV *, rep_waitreason_t));
int __rep_check_goal __P((ENV *, struct rep_waitgoal *));
int __rep_log_backup __P((ENV *, DB_LOGC *, DB_LSN *, uint32));
int __rep_get_maxpermlsn(ENV *, DB_LSN *);
int __rep_is_internal_rep_file __P((char *));
int __rep_get_datagen __P((ENV *, uint32 *));
int __rep_verify __P((ENV *, __rep_control_args *, DBT *, int, __time64_t));
int __rep_verify_fail __P((ENV *, __rep_control_args *));
int __rep_verify_req __P((ENV *, __rep_control_args *, int));
int __rep_dorecovery __P((ENV *, DB_LSN *, DB_LSN *));
int __rep_verify_match __P((ENV *, DB_LSN *, __time64_t));

#if defined(__cplusplus)
}
#endif
#endif /* !_rep_ext_h_ */