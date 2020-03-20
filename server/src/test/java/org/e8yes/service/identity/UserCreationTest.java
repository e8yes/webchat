/**
 * e8yes demo web server.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */
package org.e8yes.service.identity;

import java.sql.SQLException;
import java.time.Instant;
import org.e8yes.connection.DatabaseConnection;
import org.e8yes.environment.EnvironmentContext;
import org.e8yes.environment.Initializer;
import org.e8yes.exception.ResourceConflictException;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class UserCreationTest {

  public UserCreationTest() {}

  @BeforeAll
  public static void setUpClass() {}

  @AfterAll
  public static void tearDownClass() {}

  @BeforeEach
  public void setUp() throws SQLException, IllegalAccessException {
    Initializer.init(new EnvironmentContext(EnvironmentContext.Mode.Test));
  }

  @AfterEach
  public void tearDown() throws SQLException {
    DatabaseConnection.deleteAllData();
  }

  @Test
  public void createBaselineUserTest()
      throws ResourceConflictException, SQLException, IllegalAccessException {
    String user0PassWord = "PASS";
    UserCreation.UserEntity user = UserCreation.createBaselineUser(user0PassWord.getBytes());
    Assertions.assertNotNull(user.id.value());
    Assertions.assertNotNull(user.security_key_hash.value());
    Assertions.assertFalse(user.security_key_hash.value().isBlank());
    Assertions.assertNull(user.alias.value());
    Assertions.assertNull(user.avatar_file_id.value());
    Assertions.assertNull(user.emails.value());
    Assertions.assertNotNull(user.created_at);
    Assertions.assertTrue(
        (user.created_at.value().getTime() - Instant.now().toEpochMilli()) < 5000);
    Assertions.assertNotNull(user.active_level.value());
    Assertions.assertEquals((Integer) 0, user.active_level.value());
    Assertions.assertNotNull(user.group_names.value());
    Assertions.assertEquals(1, user.group_names.value().length);
    Assertions.assertEquals(
        SystemUserGroup.BASELINE_USER_GROUP.name(), user.group_names.value()[0]);
  }
}